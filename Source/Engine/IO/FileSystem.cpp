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
#include "IO/FileSystem.h"
#include "System/FileSystem.h"

// TODO: Implement rmrf and remove this.
#include <filesystem>

#define FILEPATH_BUFFER_SIZE 512

namespace A3D
{
bool MakeDir(const char* path)
{
	char buffer[FILEPATH_BUFFER_SIZE];
	strcpy(buffer, path);
	for (char* chr = buffer; *chr; ++chr)
		if (*chr == '/')
		{
			*chr = '\0';
			A3D_Mkdir(buffer);
			*chr = '/';
		}
	A3D_Mkdir(buffer);
	return true;
}

bool RemoveDir(const char* path)
{
	// TODO: Implement without <filesystem> module.
	return std::filesystem::remove_all(path);
}

bool IsDirExists(const char* path)
{
	return A3D_GetFileAttribute(path) == A3D_FileType::FILETYPE_DIRECTORY;
}

bool IsFileExists(const char* path)
{
	return A3D_GetFileAttribute(path) == A3D_FileType::FILETYPE_FILE;
}
} // namespace A3D
