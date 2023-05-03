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

#ifndef IO_FILESYSTEM_H
#define IO_FILESYSTEM_H

#include <stdint.h>
#include <stdio.h>
#include "A3DCoreAPI.h"

namespace A3D
{
struct File
{
	FILE* handler;
	uint32_t offset;
	uint32_t size;
};

A3DCOREAPI_EXPORT bool MakeDir(const char* path);
A3DCOREAPI_EXPORT bool RemoveDir(const char* path);

A3DCOREAPI_EXPORT bool IsDirExists(const char* path);
A3DCOREAPI_EXPORT bool IsFileExists(const char* path);

A3DCOREAPI_EXPORT bool OpenFileRead(File& file, const char* filename);
A3DCOREAPI_EXPORT bool OpenFileWrite(File& file, const char* filename);
A3DCOREAPI_EXPORT void CloseFile(File& file);
A3DCOREAPI_EXPORT bool ReadFileData(File& file, void* buffer, uint32_t size);
A3DCOREAPI_EXPORT bool WriteFileData(File& file, const void* buffer, uint32_t size);
A3DCOREAPI_EXPORT void FileRewind(File& file);

inline static bool ReadFileData(File& file, void* buffer) { return ReadFileData(file, buffer, file.size); }
} // namespace A3D

#endif // IO_FILESYSTEM_H
