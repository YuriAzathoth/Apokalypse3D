/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2023 Yuriy Zinchenko

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <string.h>
#include "IO/FileSystem.h"
#include "IO/Log.h"
#include "System/Platform.h"

#define BUFFER_SIZE 256
#define PACKAGE_EXTENSION ".pak"

#define STR_TO_I32(A, B, C, D) ((uint32_t)(D) << 24 | (uint32_t)(C) << 16U | (uint32_t)(B) << 8U | (uint32_t)(A))

#define FILEPATH_BUFFER_SIZE 1024
#define DEFAULT_BLOCK_SIZE 2048
#define MAGIC_NUMBER STR_TO_I32('.', 'P', 'A', 'K')

struct package_header_t
{
	uint32_t magic_number;
	uint32_t files_count;
};

struct file_name_meta_t
{
	uint32_t size;
	uint32_t offset;
};

struct file_body_meta_t
{
	uint32_t size;
	uint32_t offset;
};

namespace A3D
{
inline static constexpr const char* GetFileModeString(FileMode mode);

bool MakeDir(const char* path)
{
	char buffer[BUFFER_SIZE];
	strcpy(buffer, path);
	for (char* chr = buffer; *chr; ++chr)
		if (*chr == '/')
		{
			*chr = '\0';
			Mkdir(buffer);
			*chr = '/';
		}
	Mkdir(buffer);
	return true;
}

bool RemoveDir(const char* path)
{
	return Rmrf(path);
}

bool IsDirExists(const char* path)
{
	return GetFileAttribute(path) == FileType::DIRECTORY;
}

bool IsFileExists(const char* path)
{
	return GetFileAttribute(path) == FileType::FILE;
}

bool OpenFile(File& file, const char* filename, FileMode mode)
{
	LogDebug("Opening file \"%s\"...", filename);

	char filepath[BUFFER_SIZE];
	char* delim_pak;
	char* delim_path;
	bool packed = false;
	strcpy(filepath, filename);

	if (mode == FileMode::READ)
	{
		constexpr const char PACKAGE_EXT[] = PACKAGE_EXTENSION;
		const char* ext;
		char* delim_pak;

		// Check if file in package
		for (char* chr = filepath; *chr != '\0'; ++chr)
		{
			if (*chr == '/')
				delim_path = chr;
			for (ext = PACKAGE_EXT; *chr == *ext; ++ext, ++chr)
				if (*ext == '\0') // Compared full package extension
				{
					delim_pak = chr;
					packed = true;
					goto done;
				}
		}
done:

		*delim_path = '\0';
		if (GetFileAttribute(filepath) != FileType::DIRECTORY)
		{
			LogFatal("Could not open file \"%s\" for reading: directory \"%s\" does not exist.", filename, filepath);
			return false;
		}
		*delim_path = '/';

		*delim_pak = '\0';
		if (GetFileAttribute(filepath) != FileType::FILE)
		{
			LogFatal("Could not open file \"%s\" for reading: package file \"%s\" does not exist.", filename, filepath);
			return false;
		}
		*delim_pak = '/';

		if (packed)
		{
			*delim_pak = '\0';
			file.handler = fopen(filepath, "rb");
			if (file.handler == nullptr)
			{
				file.size = 0;
				LogFatal("Could not open package file \"%s\" for reading: file does not exist.", filepath);
				return false;
			}

			struct package_header_t ph;
			fread(&ph, sizeof(ph), 1, file.handler);
			if (ph.magic_number != MAGIC_NUMBER || ph.files_count == 0)
			{
				LogFatal("Failed to open package \"%s\": file is not a package.", filepath);
				fclose(file.handler);
				return -1;
			}

			char filepath_inner[FILEPATH_BUFFER_SIZE];
			struct file_name_meta_t fnm;
			struct file_body_meta_t fbm;
			uint32_t header_pos;
			for (unsigned i = 0; i < ph.files_count; ++i)
			{
				fread(&fnm, sizeof(fnm), 1, file.handler);
				fread(&fbm, sizeof(fbm), 1, file.handler);
				header_pos = (uint32_t)ftell(file.handler);

				fseek(file.handler, (long)fnm.offset, SEEK_SET);
				fread(filepath_inner, fnm.size, 1, file.handler);
				filepath_inner[fnm.size] = '\0';
				if (!strcmp(delim_pak + 1, filepath_inner))
				{
					file.offset = fbm.offset;
					return true;
				}
			}

			LogFatal("Failed to find file \"%s\" in package \"%s\": file does not exist", delim_pak + 1, filepath);
			return false;
		}
		else
		{
			file.handler = fopen(filename, "rb");
			file.offset = 0;
			if (file.handler == nullptr)
			{
				file.size = 0;
				LogFatal("Could not open file \"%s\" for reading: file does not exist.", filename);
				return false;
			}

			fseek(file.handler, 0, SEEK_END);
			file.size = (uint32_t)ftell(file.handler);
			rewind(file.handler);

			LogInfo("File \"%s\" is opened.", filename);

			return true;
		}
	}
	else
	{
		for (char* chr = filepath; *chr != '\0'; ++chr)
			if (*chr == '/')
				delim_path = chr;

		*delim_path = '\0';
		if (GetFileAttribute(filepath) != FileType::DIRECTORY)
		{
			LogFatal("Could not open file \"%s\" for writing: directory \"%s\" does not exist.", filename, filepath);
			return false;
		}
		*delim_path = '/';

		file.handler = fopen(filename, "rb");
		if (GetFileAttribute(filepath) != FileType::DIRECTORY)
		{
			LogFatal("Could not open file \"%s\" for writing: file does not exist.", filename);
			return false;
		}
		*delim_path = '/';

		file.offset = 0;
		file.size = 0;

		return true;
	}
}

void CloseFile(File& file) { fclose(file.handler); }

bool ReadFileData(File& file, void* buffer)
{
	if (file.offset > 0)
		fseek(file.handler, file.offset, SEEK_SET);
	if (fread(buffer, 1, file.size, file.handler) == file.size)
	{
		LogDebug("Read \"%u\" bytes from file.", file.size);
		return true;
	}
	else
	{
		LogFatal("Failed to read \"%u\" bytes from file.", file.size);
		return false;
	}
}

bool WriteFileData(File& file, const void* buffer, uint32_t size)
{
	if (fwrite(buffer, 1, size, file.handler) == size)
	{
		LogDebug("Write \"%u\" bytes to file.", file.size);
		return true;
	}
	else
	{
		LogFatal("Failed to write \"%u\" bytes to file.", file.size);
		return false;
	}
}

inline static constexpr const char* GetFileModeString(FileMode mode)
{
	switch (mode)
	{
	case FileMode::READ:  return "rb";
	case FileMode::WRITE: return "wb";
	}
	return nullptr;
}
} // namespace A3D
