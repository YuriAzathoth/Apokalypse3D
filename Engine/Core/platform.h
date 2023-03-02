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

#ifndef PLATFORM_H
#define PLATFORM_H

#include <stdint.h>
#include "Apokalypse3DAPI.h"

typedef uintptr_t a3d_thread_t;
typedef uintptr_t a3d_mutex_t;

typedef int(*a3d_thread_func)(void*);

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

APOKALYPSE3DAPI_EXPORT void a3d_mkdir(const char* path);

APOKALYPSE3DAPI_EXPORT a3d_thread_t a3d_thread_create(a3d_thread_func func, void* param);
APOKALYPSE3DAPI_EXPORT void a3d_thread_destroy(a3d_thread_t thread);
APOKALYPSE3DAPI_EXPORT void a3d_thread_wait(a3d_thread_t thread);

APOKALYPSE3DAPI_EXPORT a3d_mutex_t a3d_mutex_create();
APOKALYPSE3DAPI_EXPORT void a3d_mutex_destroy(a3d_mutex_t mutex);
APOKALYPSE3DAPI_EXPORT void a3d_mutex_lock(a3d_mutex_t mutex);
APOKALYPSE3DAPI_EXPORT void a3d_mutex_unlock(a3d_mutex_t mutex);

APOKALYPSE3DAPI_EXPORT void a3d_sleep(int sec, int nanosec);

#ifdef __WIN32__
void a3d_disable_high_dpi();
#else // __WIN32__
#define a3d_disable_high_dpi()
#endif // __WIN32__

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // PLATFORM_H
