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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "Container/circular_queue.h"

static constexpr unsigned TEST_ITEMS_COUNT = 256;
static constexpr unsigned TEST_RANDOM_COUNT = 2048;

TEST_SUITE("Circular Queue")
{
	TEST_CASE("Create")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		REQUIRE(queue.data != nullptr);
		REQUIRE(queue.head == queue.data);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity != 0);
		REQUIRE(queue.item_size == sizeof(unsigned));
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Push")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		*(unsigned*)a3d_circular_queue_push(&queue) = 10;
		REQUIRE(queue.data != nullptr);
		REQUIRE(queue.head > queue.data);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity != 0);
		REQUIRE(queue.item_size == sizeof(unsigned));
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Pop")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		*(unsigned*)a3d_circular_queue_push(&queue) = 20;
		REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == 20);
		REQUIRE(queue.data != nullptr);
		REQUIRE(queue.head == (unsigned*)queue.data + 1);
		REQUIRE(queue.tail == queue.head);
		REQUIRE(queue.capacity != 0);
		REQUIRE(queue.item_size == sizeof(unsigned));
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Empty")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		*(unsigned*)a3d_circular_queue_push(&queue) = 1;
		REQUIRE(a3d_circular_queue_empty(&queue) == 0);
		a3d_circular_queue_pop(&queue);
		REQUIRE(a3d_circular_queue_empty(&queue) != 0);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Push multiple no full no reserve")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		REQUIRE(queue.head == (unsigned*)queue.data + A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2);
		REQUIRE(queue.tail == queue.data);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Push multiple full no reserve")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		REQUIRE(queue.head == nullptr);
		REQUIRE(queue.tail == queue.data);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Pop multiple no full no reserve")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i);
		REQUIRE(queue.head == (unsigned*)queue.data + A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2);
		REQUIRE(queue.tail == queue.head);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Pop multiple full no reserve")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i);
		REQUIRE(queue.head == queue.data);
		REQUIRE(queue.tail == queue.head);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Reserve contiguous non-full")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < 4; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i * 2;
		a3d_circular_queue_reserve(&queue, 16);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity == 16);
		for (unsigned i = 0; i < 4; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i * 2);
		REQUIRE(queue.capacity == 16);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Reserve segmented full")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i * 2;
		a3d_circular_queue_reserve(&queue, 16);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity == 16);
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i * 2);
		REQUIRE(queue.capacity == 16);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Reserve segmented non-full")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < 6; ++i)
			a3d_circular_queue_push(&queue);
		for (unsigned i = 0; i < 6; ++i)
			a3d_circular_queue_pop(&queue);
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i * 4;
		a3d_circular_queue_reserve(&queue, 16);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity == 16);
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY / 2; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i * 4);
		REQUIRE(queue.capacity == 16);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Reserve partial full")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		a3d_circular_queue_push(&queue);
		a3d_circular_queue_pop(&queue);
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		a3d_circular_queue_reserve(&queue, 16);
		REQUIRE(queue.tail == queue.data);
		REQUIRE(queue.capacity == 16);
		for (unsigned i = 0; i < A3D_CIRCULAR_QUEUE_INITIAL_CAPACITY; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i);
		REQUIRE(queue.capacity == 16);
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Push multiple")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		a3d_circular_queue_free(&queue);
	}

	TEST_CASE("Pop multiple")
	{
		a3d_circular_queue_t queue;
		a3d_circular_queue_new(&queue, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			*(unsigned*)a3d_circular_queue_push(&queue) = i;
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			REQUIRE(*(unsigned*)a3d_circular_queue_pop(&queue) == i);
		a3d_circular_queue_free(&queue);
	}
}
