/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022 Yuriy Zinchenko

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

#include <alloca.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "Filesystem.h"

#ifdef _WIN32
//#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define MKDIR(DIR) CreateDirectory(DIR, nullptr)
#else // _WIN32
#include <sys/stat.h>
#define MKDIR(DIR) mkdir(DIR, 0774)
#endif // _WIN32

void MkDir(const char* path) noexcept
{
	char* buffer = (char*)alloca(strlen(path) + 1);
	strcpy(buffer, path);
	for (char* end = buffer; *end; ++end)
		if (*end == '/')
		{
			*end = '\0';
			MKDIR(buffer);
			*end = '/';
		}
	MKDIR(buffer);
}
