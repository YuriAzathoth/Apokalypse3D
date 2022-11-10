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

#ifndef ASYNC_QUEUE_H
#define ASYNC_QUEUE_H

#include <flecs.h>
#include "circular_queue.h"
#include "Core/platform.h"

typedef struct a3d_async_queue
{
	a3d_circular_queue_t queue;
	a3d_mutex_t sync;
} a3d_async_queue_t;

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

inline static void a3d_async_queue_new(a3d_async_queue_t* queue, unsigned item_size)
{
	a3d_circular_queue_new(&queue->queue, item_size);
	queue->sync = a3d_mutex_create();
}

inline static void a3d_async_queue_free(a3d_async_queue_t* queue)
{
	a3d_circular_queue_free(&queue->queue);
	a3d_mutex_destroy(queue->sync);
}

inline static int a3d_async_queue_empty(a3d_async_queue_t* queue)
{
	a3d_mutex_lock(queue->sync);
	const int res = a3d_circular_queue_empty(&queue->queue);
	a3d_mutex_unlock(queue->sync);
	return res;
}

inline static void a3d_async_queue_reserve(a3d_async_queue_t* queue, unsigned count)
{
	a3d_mutex_lock(queue->sync);
	a3d_circular_queue_reserve(&queue->queue, count);
	a3d_mutex_unlock(queue->sync);
}

inline static void a3d_async_queue_push(a3d_async_queue_t* queue, const void* src)
{
	a3d_mutex_lock(queue->sync);
	memcpy(a3d_circular_queue_push(&queue->queue), src, queue->queue.item_size);
	a3d_mutex_unlock(queue->sync);
}

inline static int a3d_async_queue_pop(a3d_async_queue_t* queue, void* dst)
{
	a3d_mutex_lock(queue->sync);
	const int ret = !a3d_circular_queue_empty(&queue->queue);
	if (ret)
	{
		void* ptr = a3d_circular_queue_pop(&queue->queue);
		memcpy(dst, ptr, queue->queue.item_size);
	}
	a3d_mutex_unlock(queue->sync);
	return ret;
}

inline static void a3d_async_queue_clear(a3d_async_queue_t* queue)
{
	a3d_mutex_lock(queue->sync);
	a3d_circular_queue_clear(&queue->queue);
	a3d_mutex_unlock(queue->sync);
}

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // ASYNC_QUEUE_H
