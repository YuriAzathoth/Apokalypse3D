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

#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define A3D_VECTOR_INITIAL_CAPACITY 2
#define A3D_VECTOR_GROW_FACTOR 2

typedef struct a3d_vector
{
	void* data;
	unsigned count;
	unsigned capacity;
	unsigned item_size;
} a3d_vector_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline static void a3d_vector_new(a3d_vector_t* vector, unsigned item_size)
{
	vector->data = NULL;
	vector->count = 0;
	vector->capacity = 0;
	vector->item_size = item_size;
}

inline static void a3d_vector_free(a3d_vector_t* vector)
{
	free(vector->data);
}

inline static void a3d_vector_reserve(a3d_vector_t* vector, unsigned count)
{
	if (count > vector->capacity)
	{
		const unsigned new_capacity = count * vector->item_size;
		void* new_data = malloc(count * vector->item_size);
		memcpy(new_data, vector->data, vector->count * vector->item_size);
		free(vector->data);
		vector->data = new_data;
		vector->capacity = count;
	}
}

inline static void a3d_vector_shrink(a3d_vector_t* vector)
{
	if (vector->count > 0)
	{
		if (vector->count < vector->capacity)
		{
			void* new_data = malloc(vector->count * vector->item_size);
			memcpy(new_data, vector->data, vector->count * vector->item_size);
			free(vector->data);
			vector->data = new_data;
			vector->capacity = vector->count;
		}
	}
	else
	{
		free(vector->data);
		vector->data = NULL;
		vector->capacity = 0;
	}
}

inline static void a3d_vector_clear(a3d_vector_t* vector)
{
	vector->count = 0;
}

inline static void* a3d_vector_get(const a3d_vector_t* vector, unsigned id)
{
	return (uint8_t*)vector->data + (uintptr_t)id * vector->item_size;
}

inline static void* a3d_vector_first(const a3d_vector_t* vector)
{
	return vector->data;
}

inline static void* a3d_vector_last(const a3d_vector_t* vector)
{
	return (uint8_t*)vector->data + (uintptr_t)(vector->count - 1) * vector->item_size;
}

inline static void* a3d_vector_push(a3d_vector_t* vector)
{
	if (vector->count == vector->capacity)
		a3d_vector_reserve(vector,
						   vector->capacity > 0 ?
						   vector->capacity * A3D_VECTOR_GROW_FACTOR :
						   A3D_VECTOR_INITIAL_CAPACITY);
	void* ptr = (uint8_t*)vector->data + (uintptr_t)vector->count * vector->item_size;
	++vector->count;
	return ptr;
}

inline static void a3d_vector_pop(a3d_vector_t* vector)
{
	--vector->count;
}

inline static void a3d_vector_erase_swap(a3d_vector_t* vector, unsigned i)
{
	if (i < vector->count)
		memcpy(a3d_vector_get(vector, i), a3d_vector_last(vector), vector->item_size);
	a3d_vector_pop(vector);
}

inline static void a3d_vector_resize(a3d_vector_t* vector, unsigned count, void* data)
{
	if (count > vector->count)
	{
		a3d_vector_reserve(vector, count);
		for (uint8_t* ptr = (uint8_t*)vector->data + vector->count * vector->item_size;
			ptr < (uint8_t*)vector->data + count * vector->item_size;
			ptr += vector->item_size)
			memcpy(ptr, data, vector->item_size);
	}
	vector->count = count;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // VECTOR_H
