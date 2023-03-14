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

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // __WIN32__
#include <sys/stat.h>
#endif // __WIN32__

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

#ifdef __WIN32__
#define _mkdir(filepath) CreateDirectory(filepath, NULL);
int dir_exists(const char* filepath)
{
	const DWORD attrib = GetFileAttributes(filepath);
	return attrib & FILE_ATTRIBUTE_DIRECTORY;
}
#else // __WIN32__
#define _mkdir(filepath) mkdir(filepath, 0774);
int dir_exists(const char* filepath)
{
	struct stat sb;
	return (stat(filename, &sb) != 0) && S_ISDIR(sb.st_mode);
}
#endif // __WIN32__

void mkdirr(char* path)
{
	for (char* chr = path; *chr; ++chr)
		if (*chr == '/')
		{
			*chr = '\0';
			_mkdir(path);
			*chr = '/';
		}
	_mkdir(path);
}

static int pack_files(const char* package_name, const char** files, unsigned files_count, unsigned block_size)
{
	FILE* package = fopen(package_name, "wb");
	if (package == NULL)
	{
		fprintf(stderr, "Failed to create package \"%s\": access denied.", package_name);
		return -1;
	}

	char* buffer = (char*)malloc(block_size);
	if (buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate \"%u\" bytes for read buffer: out of memory.", block_size);
		fclose(package);
		return -1;
	}

	struct package_header_t ph;
	ph.magic_number = MAGIC_NUMBER;
	ph.files_count = files_count;
	fwrite(&ph, sizeof(ph), 1, package);

	struct file_name_meta_t fnm;
	struct file_body_meta_t fbm;
	uint32_t header_pos = (uint32_t)ftell(package);
	uint32_t body_pos = (uint32_t)(sizeof(ph) + (sizeof(fnm) + sizeof(fbm)) * files_count);

	const uint8_t ZERO = 0;
	fwrite(&ZERO, sizeof(ZERO), body_pos, package);

	const char** filename;
	for (filename = files; filename < files + files_count; ++filename)
	{
		fnm.size = (uint32_t)strlen(*filename);
		fnm.offset = body_pos;
		fbm.size = 0xFFFFFFFF;
		fbm.offset = 0xFFFFFFFF;

		fseek(package, (long)body_pos, SEEK_SET);
		fwrite(*filename, fnm.size, 1, package);
		body_pos = (uint32_t)ftell(package);

		fseek(package, (long)header_pos, SEEK_SET);
		fwrite(&fnm, sizeof(fnm), 1, package);
		fwrite(&fbm, sizeof(fbm), 1, package);
		header_pos = (uint32_t)ftell(package);
	}

	header_pos = (uint32_t)sizeof(ph);
	fseek(package, (long)header_pos, SEEK_SET);

	FILE* file;
	uint32_t size_remains;
	uint32_t size_read;
	for (filename = files; filename < files + files_count; ++filename)
	{
		file = fopen(*filename, "rb");
		if (file == NULL)
		{
			fprintf(stderr, "Failed to open file \"%s\" for reading: file not found.", *filename);
			fclose(package);
			free(buffer);
			return -1;
		}

		fseek(file, 0, SEEK_END);
		fbm.size = (uint32_t)ftell(file);
		fbm.offset = body_pos;
		rewind(file);

		fseek(package, (long)header_pos, SEEK_SET);
		fseek(package, (long)sizeof(fnm), SEEK_CUR);
		fwrite(&fbm, sizeof(fbm), 1, package);
		header_pos = (uint32_t)ftell(package);

		fseek(package, (long)body_pos, SEEK_SET);

		size_remains = fbm.size;
		while (size_remains > 0)
		{
			size_read = (size_remains > block_size) ? block_size : size_remains;
			size_remains -= size_read;

			if (fread(buffer, size_read, 1, file) != 1)
			{
				fprintf(stderr, "Failed to read %u bytes from file \"%s\".", size_read, *filename);
				fclose(file);
				fclose(package);
				free(buffer);
				return -1;
			}

			if (fwrite(buffer, size_read, 1, package) != 1)
			{
				fprintf(stderr, "Failed to write %u bytes to package \"%s\".", size_read, package_name);
				fclose(file);
				fclose(package);
				free(buffer);
				return -1;
			}
		}

		fclose(file);

		body_pos = (uint32_t)ftell(package);
	}

	free(buffer);
	fclose(package);

	printf("%u files are successfully packed to package \"%s\".", files_count, package_name);

	return files_count;
}

static int unpack_files(const char* package_name, const char* directory, unsigned block_size)
{
	FILE* package = fopen(package_name, "rb");
	if (package == NULL)
	{
		fprintf(stderr, "Failed to open package \"%s\": file does not exist.", package_name);
		return -1;
	}

	struct package_header_t ph;
	fread(&ph, sizeof(ph), 1, package);
	if (ph.magic_number != MAGIC_NUMBER || ph.files_count == 0)
	{
		fprintf(stderr, "Failed to open package \"%s\": file is not a package.", package_name);
		fclose(package);
		return -1;
	}

	char* buffer = (char*)malloc(block_size);
	if (buffer == NULL)
	{
		fprintf(stderr, "Failed to allocate \"%u\" bytes for read buffer: out of memory.", block_size);
		fclose(package);
		return -1;
	}

	struct file_name_meta_t fnm;
	struct file_body_meta_t fbm;
	char filepath[FILEPATH_BUFFER_SIZE];
	char* filepath_delim = strcpy(filepath, directory);
	FILE* file;
	uint32_t header_pos;
	uint32_t size_remains;
	uint32_t size_read;
	for (unsigned i = 0; i < ph.files_count; ++i)
	{
		fread(&fnm, sizeof(fnm), 1, package);
		fread(&fbm, sizeof(fbm), 1, package);
		header_pos = (uint32_t)ftell(package);

		fseek(package, (long)fnm.offset, SEEK_SET);
		fread(filepath_delim, fnm.size, 1, package);
		filepath_delim[fnm.size] = '\0';
		fseek(package, (long)fbm.offset, SEEK_SET);

		file = fopen(filepath, "wb");
		if (file == NULL)
		{
			fprintf(stderr, "Failed to open file \"%s\" for writting: access denied.", filepath);
			fclose(package);
			free(buffer);
			return -1;
		}

		size_remains = fbm.size;
		while (size_remains > 0)
		{
			size_read = (size_remains > block_size) ? block_size : size_remains;
			size_remains -= size_read;

			if (fread(buffer, size_read, 1, package) != 1)
			{
				fprintf(stderr, "Failed to read %u bytes from package \"%s\".", size_read, package_name);
				fclose(file);
				fclose(package);
				free(buffer);
				return -1;
			}

			if (fwrite(buffer, size_read, 1, file) != 1)
			{
				fprintf(stderr, "Failed to write %u bytes to file \"%s\".", size_read, filepath);
				fclose(file);
				fclose(package);
				free(buffer);
				return -1;
			}
		}

		*filepath_delim = '\0';
		fclose(file);

		fseek(package, (long)header_pos, SEEK_SET);
	}

	fclose(package);
	free(buffer);
	return 1;
}

void print_help()
{
	printf("Usage: packer mode [options]\n");
	printf("Mode:\n");
	printf("  p package files...     Pack files to package;\n");
	printf("  u package directory    Unpack package files to directory;\n");
	printf("  --help                 Show this text.\n");
}

int main(int argc, char** argv)
{
	if (argc < 4)
	{
		print_help();
		return 1;
	}

	if (argv[1][0] == 'p' && argv[1][1] == '\0') // Pack files
	{
		const char* package = argv[2];
		const char** files = (const char**)&argv[3];
		const int files_count = argc - 3;
		const unsigned block_size = DEFAULT_BLOCK_SIZE;
		return pack_files(package, files, files_count, block_size) > 0;
	}
	else if (argv[1][0] == 'u' && argv[1][1] == '\0') // Unpack files
	{
		const char* package = argv[2];
		const char* directory = argv[3];
		const unsigned block_size = DEFAULT_BLOCK_SIZE;
		return unpack_files(package, directory, block_size) > 0;
	}
	else if (!strcmp(argv[1], "--help"))
	{
		print_help();
		return 0;
	}
	else
	{
		fprintf(stderr, "Unknown mode: %s\n.", argv[1]);
		print_help();
		return 1;
	}

	//for (const char** argp = argv; argp < argv + )

	return 0;
}
