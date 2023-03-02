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

#ifndef DYNARRAY_H
#define DYNARRAY_H

#include <stdlib.h>
#include <string.h>

typedef struct a3d_dynarray
{
	void* data;
	unsigned count;
	unsigned item_size;
} a3d_dynarray_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline static void a3d_dynarray_new(a3d_dynarray_t* dynarray, unsigned item_size, unsigned count)
{
	if (count > 0)
		dynarray->data = malloc(item_size * count);
	else
		dynarray->data = NULL;
	dynarray->count = count;
	dynarray->item_size = item_size;
}

inline static void a3d_dynarray_free(a3d_dynarray_t* dynarray)
{
	free(dynarray->data);
}

inline static void* a3d_dynarray_get(const a3d_dynarray_t* dynarray, unsigned id)
{
	return (unsigned char*)dynarray->data + (uintptr_t)id * dynarray->item_size;
}

inline static void* a3d_dynarray_first(const a3d_dynarray_t* dynarray)
{
	return dynarray->data;
}

inline static void* a3d_dynarray_last(const a3d_dynarray_t* dynarray)
{
	return a3d_dynarray_get(dynarray, dynarray->count - 1);
}

inline static void* a3d_dynarray_begin(const a3d_dynarray_t* dynarray)
{
	return dynarray->data;
}

inline static void* a3d_dynarray_end(const a3d_dynarray_t* dynarray)
{
	return a3d_dynarray_get(dynarray, dynarray->count);
}

inline static void* a3d_dynarray_first_redundant(const a3d_dynarray_t* dynarray, unsigned new_count)
{
	if (new_count < dynarray->count)
		return a3d_dynarray_get(dynarray, new_count + 1);
	else
		return a3d_dynarray_end(dynarray);
}

inline static void* a3d_dynarray_first_insufficient(const a3d_dynarray_t* dynarray, unsigned old_count)
{
	if (dynarray->count > old_count)
		return a3d_dynarray_get(dynarray, old_count + 1);
	else
		return a3d_dynarray_end(dynarray);
}

inline static void a3d_dynarray_resize(a3d_dynarray_t* dynarray, unsigned count)
{
	void* new_data = NULL;
	if (count > 0)
	{
		const unsigned new_size = count * dynarray->item_size;
		new_data = malloc(new_size);
		if (dynarray->count > 0)
		{
			const unsigned old_size = dynarray->count * dynarray->item_size;
			memcpy(new_data, dynarray->data, (new_size < old_size) ? new_size : old_size);
		}
	}
	if (dynarray->count > 0)
		free(dynarray->data);
	dynarray->data = new_data;
	dynarray->count = count;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // DYNARRAY_H
