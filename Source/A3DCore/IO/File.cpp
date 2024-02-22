/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2024 Yuriy Zinchenko

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
#include "Core/EngineLog.h"
#include "IO/File.h"
#include "IO/FileSystem.h"
#include "System/Platform.h"

#define PACKAGE_EXTENSION ".pak"

#define STR_TO_I32(A, B, C, D) ((uint32_t)(D) << 24 | (uint32_t)(C) << 16U | (uint32_t)(B) << 8U | (uint32_t)(A))

#define FILEPATH_BUFFER_SIZE 512
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
bool OpenFileRead(File& file, const char* filename)
{
	if (IsFileExists(filename))
	{
		LogDebug("Opening file \"%s\" for reading from disk...", filename);

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

		LogDebug("File \"%s\" is successfully loaded.", filename);
		return true;
	}

	char filepath[FILEPATH_BUFFER_SIZE];
	strcpy(filepath, filename);

	// File not found. Try to find in package.
	constexpr const char PACKAGE_EXT[] = PACKAGE_EXTENSION;
	char temp[5];
	char* delim_path = nullptr;
	char* chr = filepath;
	unsigned i;
	bool packed = false;
	bool exist;

	// Skip suffices like "/", "./", "../" etc.
	while (*chr == '.' || *chr == '/')
		++chr;

	// Check if package exists.
	while (*chr != '\0')
	{
		if (*chr == '/')
		{
			*chr = '\0';
			exist = IsDirExists(filepath);
			*chr = '/';

			for (i = 0; i < 5; ++i)
			{
				temp[i] = chr[i];
				chr[i] = PACKAGE_EXT[i];
			}
			LogDebug("Checking if package \"%s\" exists...", filepath);

			if (IsFileExists(filepath))
			{
				LogDebug("Package \"%s\" exist. Opening package...", filepath);

				file.handler = fopen(filepath, "rb");
				if (file.handler == nullptr)
				{
					LogFatal("Could not open package file \"%s\" for reading: access denied.", filepath);
					file.size = 0;
					return false;
				}

				packed = true;
				delim_path = chr;
			}
			for (i = 0; i < 5; ++i)
				chr[i] = temp[i];

			if (packed)
				break;

			// Neither directory nor package file exist.
			if (!exist)
			{
				LogFatal("Could not open file \"%s\" for reading: neither directory \"%s\" nor package does not exist.",
						 filename, filepath);
				return false;
			}
		}
		++chr;
	}

	if (packed) // File is in package.
	{
		*delim_path = '\0';
		++delim_path;

		LogDebug("Finding file \"%s\" in package \"%s\"...", delim_path, filepath);

		struct package_header_t ph;
		fread(&ph, sizeof(ph), 1, file.handler);
		if (ph.magic_number != MAGIC_NUMBER || ph.files_count == 0)
		{
			LogFatal("Failed to open package \"%s\": file is not a package.", filepath);
			fclose(file.handler);
			return false;
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
			fseek(file.handler, (long)header_pos, SEEK_SET);

			if (!strcmp(delim_path, filepath_inner))
			{
				fseek(file.handler, (long)fbm.offset, SEEK_SET);
				file.offset = fbm.offset;
				file.size = fbm.size;
				return true;
			}
		}
	}

	// File is not in package and directory does not exist.
	LogFatal("Failed to find file \"%s\": directory does not exist.", filename);
	return false;
}

bool OpenFileWrite(File& file, const char* filename)
{
	LogDebug("Opening file \"%s\" for writing...", filename);

	char filepath[FILEPATH_BUFFER_SIZE];
	char* delim_path;
	strcpy(filepath, filename);

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

void CloseFile(File& file) { fclose(file.handler); }

bool ReadFileData(File& file, void* buffer, uint32_t size)
{
	if (fread(buffer, size, 1, file.handler) == 1)
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
	if (fwrite(buffer, size, 1, file.handler) == 1)
	{
		LogDebug("Write \"%u\" bytes to file.", file.size);
		file.size += size;
		return true;
	}
	else
	{
		LogFatal("Failed to write \"%u\" bytes to file.", file.size);
		return false;
	}
}
} // namespace A3D
