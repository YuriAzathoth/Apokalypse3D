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

#ifndef CIRCULAR_QUEUE_H
#define CIRCULAR_QUEUE_H

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY 8
#define A3D_CIRCULAR_QUEUE_GROW_FACTOR 2

typedef struct a3d_circular_queue
{
	void* data;
	void* head;
	void* tail;
	unsigned capacity;
	unsigned item_size;
} a3d_circular_queue_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline static void a3d_circular_queue_new(a3d_circular_queue_t* queue, unsigned item_size)
{
	queue->data = malloc(item_size * A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY);
	queue->head = queue->data;
	queue->tail = queue->data;
	queue->capacity = A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY;
	queue->item_size = item_size;
}

inline static void a3d_circular_queue_free(a3d_circular_queue_t* queue)
{
	free(queue->data);
}

inline static int a3d_circular_queue_empty(a3d_circular_queue_t* queue)
{
	return queue->head == queue->tail;
}

inline static void a3d_circular_queue_reserve(a3d_circular_queue_t* queue, unsigned count)
{
	unsigned size = 0;
	uint8_t* new_data = (uint8_t*)malloc(count * queue->item_size);
	if (queue->head != queue->tail)
	{
		const int full = queue->head == NULL;
		uint8_t* buffer_begin = (uint8_t*)queue->data;
		uint8_t* buffer_end = (uint8_t*)queue->data + queue->capacity * queue->item_size;
		uint8_t* buffer_last = buffer_end - queue->item_size;
		uint8_t* data_first = (uint8_t*)queue->tail;
		uint8_t* data_last = !full ?
			((queue->head > buffer_begin) ?
				(uint8_t*)queue->head - queue->item_size :
				buffer_last) :
			(uint8_t*)queue->tail - queue->item_size;
		if (data_last > data_first)
		{
			size = (unsigned)((uint8_t*)data_last - (uint8_t*)data_first) + queue->item_size;
			memcpy(new_data, data_first, size);
		}
		else
		{
			const unsigned tail_to_end = buffer_end - data_first;
			const unsigned begin_to_head = data_last - buffer_begin + queue->item_size;
			size = tail_to_end + begin_to_head;
			memcpy(new_data, data_first, tail_to_end);
			memcpy(new_data + tail_to_end, queue->data, begin_to_head);
		}
		free(queue->data);
	}
	queue->data = new_data;
	queue->tail = new_data;
	queue->head = (uint8_t*)new_data + size;
	queue->capacity = count;
}

inline static void* a3d_circular_queue_push(a3d_circular_queue_t* queue)
{
	if (queue->head == NULL)
		a3d_circular_queue_reserve(queue, queue->capacity * A3D_CIRCULAR_QUEUE_GROW_FACTOR);
	void* ptr = queue->head;
	queue->head = (queue->head <
		(uint8_t*)queue->data + (queue->capacity - 1) * queue->item_size) ?
		(uint8_t*)queue->head + queue->item_size :
		queue->data;
	if (queue->head == queue->tail)
		queue->head = NULL;
	return ptr;
}

inline static void* a3d_circular_queue_pop(a3d_circular_queue_t* queue)
{
	if (queue->head == NULL)
		queue->head = queue->tail;
	void* ptr = queue->tail;
	queue->tail = (queue->tail <
		(uint8_t*)queue->data + (queue->capacity - 1) * queue->item_size) ?
		(uint8_t*)queue->tail + queue->item_size :
		queue->data;
	return ptr;
}

inline static void a3d_circular_queue_clear(a3d_circular_queue_t* queue)
{
	queue->head = queue->data;
	queue->tail = queue->data;
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // CIRCULAR_QUEUE_H
