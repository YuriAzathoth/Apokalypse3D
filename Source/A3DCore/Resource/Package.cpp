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

#include <stdio.h>
#include <stdint.h>
#include <unordered_map>
#include "Container/dense_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "Core/EngineLog.h"
#include "Package.h"
#include "System/Platform.h"

#define BUFFER_SIZE 256
#define STR_TO_I32(A, B, C, D) ((uint32_t)(D) << 24U | (uint32_t)(C) << 16U | (uint32_t)(B) << 8U | (uint32_t)(A))

inline static constexpr uint32_t MAGIC_NUMBER = STR_TO_I32('P', 'A', 'K', ' ');

namespace A3D
{
using FilenameIndex = uint32_t;
using PackageIndex = uint16_t;

struct PackageHeader
{
	uint32_t magic_number;
	uint32_t files_count;
};

struct FileNameMeta
{
	uint32_t size;
	uint32_t offset;
};

struct FileBodyMeta
{
	uint32_t size;
	uint32_t offset;
};

struct PackageCache
{
	std::unordered_map<string_hash, PackageIndex> ids;

	dense_map<PackageIndex, vector<FilenameIndex, FilenameIndex>> files;
	dense_map<PackageIndex, string> filenames;
};

struct PackageFileCache
{
	std::unordered_map<string_hash, FilenameIndex> ids;

	dense_map<FilenameIndex, PackageIndex> packages;
	dense_map<FilenameIndex, string> filenames;
	dense_map<FilenameIndex, uint32_t> offsets;
	dense_map<FilenameIndex, uint32_t> sizes;
};

static PackageCache g_package_cache;
static PackageFileCache g_filename_cache;

bool AddPackage(const char* filename)
{
	const string_hash filename_hash(filename);
	const auto it = g_package_cache.ids.find(filename_hash);
	if (it != g_package_cache.ids.end())
		return true;

	LogDebug("Loading package file \"%s\"...", filename);

	FILE* file = fopen(filename, "rb");
	if (file == nullptr)
	{
		LogFatal("Failed to add package file \"%s\": file not found.", filename);
		return false;
	}

	struct PackageHeader ph;
	fread(&ph, sizeof(ph), 1, file);
	if (ph.magic_number != MAGIC_NUMBER || ph.files_count == 0)
	{
		LogFatal("Failed to open package \"%s\": file is not a package.", filename);
		fclose(file);
		return false;
	}

	const PackageIndex package_index = g_package_cache.files.size();

	vector<FilenameIndex, FilenameIndex> files;
	char file_path[BUFFER_SIZE];
	struct FileNameMeta fnm;
	struct FileBodyMeta fbm;
	uint32_t header_pos;
	PackageIndex file_index;
	for (unsigned i = 0; i < ph.files_count; ++i)
	{
		fread(&fnm, sizeof(fnm), 1, file);
		fread(&fbm, sizeof(fbm), 1, file);

		if (fnm.size + 1 > BUFFER_SIZE)
		{
			LogFatal("Could not load package file \"%s\": containing file name is longer than 256.", filename);
			fclose(file);
			return false;
		}

		header_pos = (uint32_t)ftell(file);
		fseek(file, (long)fnm.offset, SEEK_SET);
		fread(file_path, fnm.size, 1, file);
		file_path[fnm.size] = '\0';
		fseek(file, (long)header_pos, SEEK_SET);

		file_index = g_filename_cache.filenames.size();
		files.push_back(file_index);

		g_filename_cache.ids.emplace(file_path, g_filename_cache.filenames.size());
		g_filename_cache.packages.insert(package_index);
		g_filename_cache.filenames.insert(file_path);
		g_filename_cache.sizes.insert(fbm.size);
		g_filename_cache.offsets.insert(fbm.offset);
	}

	fclose(file);

	g_package_cache.ids.emplace(filename_hash, package_index);
	g_package_cache.filenames.insert(filename);
	g_package_cache.files.emplace(std::move(files));

	LogInfo("Package file \"%s\" is successfully loaded.", filename);

	return true;
}
} // namespace A3D
