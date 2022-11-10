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

#include <doctest/doctest.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include "Container/async_queue.h"

static constexpr unsigned TEST_ITEMS_COUNT = 256;
static constexpr unsigned TEST_RANDOM_COUNT = 2048;

TEST_SUITE("Circular Queue")
{
	TEST_CASE("Create")
	{
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		REQUIRE(queue.queue.data != nullptr);
		REQUIRE(queue.queue.head == queue.queue.data);
		REQUIRE(queue.queue.tail == queue.queue.data);
		REQUIRE(queue.queue.capacity != 0);
		REQUIRE(queue.queue.item_size == sizeof(unsigned));
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Push")
	{
		unsigned val = 10;
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		a3d_async_queue_push(&queue, &val);
		REQUIRE(queue.queue.data != nullptr);
		REQUIRE(queue.queue.head > queue.queue.data);
		REQUIRE(queue.queue.tail == queue.queue.data);
		REQUIRE(queue.queue.capacity != 0);
		REQUIRE(queue.queue.item_size == sizeof(unsigned));
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Pop")
	{
		unsigned val = 20;
		unsigned res;
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		a3d_async_queue_push(&queue, &val);
		a3d_async_queue_pop(&queue, &res);
		REQUIRE(res == val);
		REQUIRE(queue.queue.data != nullptr);
		REQUIRE(queue.queue.head == (unsigned*)queue.queue.data + 1);
		REQUIRE(queue.queue.tail == queue.queue.head);
		REQUIRE(queue.queue.capacity != 0);
		REQUIRE(queue.queue.item_size == sizeof(unsigned));
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Pop empty")
	{
		unsigned res;
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		REQUIRE(a3d_async_queue_pop(&queue, &res) == 0);
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Empty")
	{
		unsigned val = 1;
		unsigned ret;
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		a3d_async_queue_push(&queue, &val);
		REQUIRE(a3d_async_queue_empty(&queue) == 0);
		a3d_async_queue_pop(&queue, &ret);
		REQUIRE(a3d_async_queue_empty(&queue) != 0);
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Push multiple")
	{
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			a3d_async_queue_push(&queue, &i);
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Pop multiple")
	{
		unsigned ret;
		a3d_async_queue_t queue;
		a3d_async_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			a3d_async_queue_push(&queue, &i);
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
		{
			a3d_async_queue_pop(&queue, &ret);
			REQUIRE(ret == i);
		}
		a3d_async_queue_free(&queue);
	}

	TEST_CASE("Single producer single consumer")
	{
//		a3d_async_queue_t queue;
//		a3d_async_queue_new(&queue, sizeof(unsigned));
//		std::thread producer([&queue]()
//		{
//			for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
//				a3d_async_queue_push(&queue, &i);
//		});
//		std::thread consumer([&queue]()
//		{
//			unsigned val;
//			unsigned i = 0;
//			while (i < TEST_ITEMS_COUNT)
//			{
//				if (a3d_async_queue_pop(&queue, &val) != 0)
//				{
//					REQUIRE(val == i);
//					++i;
//				}
//			}
//		});
//		producer.join();
//		consumer.join();
//		a3d_async_queue_free(&queue);
	}
}
