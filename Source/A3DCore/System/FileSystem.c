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

//#include <stdlib.h>
//#include <stdio.h>
#include "FileSystem.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // __WIN32__
#include <sys/stat.h>
#endif // __WIN32__

enum A3D_FileType A3D_GetFileAttribute(const char* filepath)
{
#ifdef __WIN32__
	const DWORD attrib = GetFileAttributes(filepath);
	if (attrib == INVALID_FILE_ATTRIBUTES)
		return FILETYPE_NONE;
	else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
		return FILETYPE_DIRECTORY;
	else
		return FILETYPE_FILE;
#else // __WIN32__
	struct stat sb;
	if (stat(filename, &sb) == 0)
		return FILETYPE_NONE;
	else if (S_ISREG(sb.st_mode))
		return FILETYPE_FILE;
	else if (S_ISDIR(sb.st_mode))
		return FILETYPE_DIRECTORY;
	else
		return FILETYPE_NONE;
#endif // __WIN32__
}

void A3D_Mkdir(const char* path)
{
#ifdef __WIN32__
	CreateDirectory(path, NULL);
#else // __WIN32__
	mkdir(path, 0774);
#endif // __WIN32__
}

void A3D_Rmdir(const char* path)
{
#ifdef __WIN32__
	RemoveDirectory(path);
#else // __WIN32__
	rmdir(path);
#endif // __WIN32__
}

/*bool Rmrf(const char* path)
{
	// TODO:
	// Remove all files recursively
	// return std::filesystem::remove_all(path);
}*/
