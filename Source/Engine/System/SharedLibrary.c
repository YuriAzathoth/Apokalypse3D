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

#include <stdlib.h>
#include "SharedLibrary.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void* A3D_LoadLibrary(const char* path)
{
	HINSTANCE lib = LoadLibrary((LPSTR)path);
	return (void*)lib;
}

void A3D_CloseLibrary(void* library)
{
	FreeLibrary((HINSTANCE)library);
}

void* A3D_GetLibrarySymbol(void* library, const char* name)
{
	return (void*)GetProcAddress((HINSTANCE)library, (LPSTR)name);
}
#else // __WIN32__
#include <dlfcn.h>

void* A3D_LoadLibrary(const char* path)
{
	void* lib = dlopen(path, RTLD_LAZY);
	return lib;
}

void A3D_CloseLibrary(void* library)
{
	dlclose(library);
}

void* A3D_GetLibrarySymbol(void* library, const char* name)
{
	return (void*)dlsym(library, name);
}

#endif // __WIN32__
