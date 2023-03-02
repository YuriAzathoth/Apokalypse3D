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
#include <utility>
#include "Container/vector.h"

static constexpr unsigned TEST_ITEMS_COUNT = 256;

TEST_SUITE("Vector")
{
	TEST_CASE("Create")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		REQUIRE(vector.data == nullptr);
		REQUIRE(vector.capacity == 0);
		REQUIRE(vector.count == 0);
		REQUIRE(vector.item_size == sizeof(unsigned));
		a3d_vector_free(&vector);
	}

	TEST_CASE("Reserve")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		a3d_vector_reserve(&vector, 10);
		REQUIRE(vector.capacity == 10);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Reserve greater")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		a3d_vector_reserve(&vector, 8);
		a3d_vector_reserve(&vector, 20);
		REQUIRE(vector.capacity == 20);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Reserve less")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		a3d_vector_reserve(&vector, 12);
		a3d_vector_reserve(&vector, 8);
		REQUIRE(vector.capacity == 12);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Shrink to zero")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		a3d_vector_reserve(&vector, 5);
		a3d_vector_shrink(&vector);
		REQUIRE(vector.capacity == 0);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Push")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		unsigned* ptr = (unsigned*)a3d_vector_push(&vector);
		REQUIRE(ptr != nullptr);
		REQUIRE(vector.data != nullptr);
		REQUIRE(vector.capacity != 0);
		REQUIRE(vector.count == 1);
		*ptr = 12;
		REQUIRE(*(unsigned*)a3d_vector_last(&vector) == 12);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Pop")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 28;
		a3d_vector_pop(&vector);
		REQUIRE(vector.capacity > 0);
		REQUIRE(vector.count == 0);
		a3d_vector_free(&vector);
	}

	TEST_CASE("First")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 10;
		*(unsigned*)a3d_vector_push(&vector) = 20;
		REQUIRE(*(unsigned*)a3d_vector_first(&vector) == 10);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Last")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 10;
		*(unsigned*)a3d_vector_push(&vector) = 20;
		REQUIRE(*(unsigned*)a3d_vector_last(&vector) == 20);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Push multi")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			*(unsigned*)a3d_vector_push(&vector) = i;
		REQUIRE(vector.count == TEST_ITEMS_COUNT);
		REQUIRE(vector.capacity == TEST_ITEMS_COUNT);
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			REQUIRE(*(unsigned*)a3d_vector_get(&vector, i) == i);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Pop multi")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		for (unsigned i = 0; i < TEST_ITEMS_COUNT; ++i)
			*(unsigned*)a3d_vector_push(&vector) = i;
		for (unsigned i = 0; i < TEST_ITEMS_COUNT / 2; ++i)
			a3d_vector_pop(&vector);
		REQUIRE(vector.count == TEST_ITEMS_COUNT / 2);
		REQUIRE(vector.capacity == TEST_ITEMS_COUNT);
		for (unsigned i = 0; i < TEST_ITEMS_COUNT / 2; ++i)
			REQUIRE(*(unsigned*)a3d_vector_get(&vector, i) == i);
		for (unsigned i = 0; i < TEST_ITEMS_COUNT / 2; ++i)
			a3d_vector_pop(&vector);
		REQUIRE(vector.count == 0);
		REQUIRE(vector.capacity == TEST_ITEMS_COUNT);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Erase swap middle")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 0;
		*(unsigned*)a3d_vector_push(&vector) = 1;
		*(unsigned*)a3d_vector_push(&vector) = 2;
		*(unsigned*)a3d_vector_push(&vector) = 3;
		*(unsigned*)a3d_vector_push(&vector) = 4;
		a3d_vector_erase_swap(&vector, 2);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 0) == 0);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 1) == 1);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 2) == 4);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 3) == 3);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Erase swap last")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 0;
		*(unsigned*)a3d_vector_push(&vector) = 1;
		*(unsigned*)a3d_vector_push(&vector) = 2;
		*(unsigned*)a3d_vector_push(&vector) = 3;
		*(unsigned*)a3d_vector_push(&vector) = 4;
		a3d_vector_erase_swap(&vector, 4);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 0) == 0);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 1) == 1);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 2) == 2);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 3) == 3);
		a3d_vector_free(&vector);
	}

	TEST_CASE("Resize")
	{
		a3d_vector_t vector;
		a3d_vector_new(&vector, sizeof(unsigned));
		*(unsigned*)a3d_vector_push(&vector) = 0;
		*(unsigned*)a3d_vector_push(&vector) = 1;
		*(unsigned*)a3d_vector_push(&vector) = 2;
		*(unsigned*)a3d_vector_push(&vector) = 3;
		*(unsigned*)a3d_vector_push(&vector) = 4;
		unsigned val = 8;
		a3d_vector_resize(&vector, 10, &val);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 0) == 0);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 1) == 1);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 2) == 2);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 3) == 3);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 4) == 4);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 5) == 8);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 6) == 8);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 7) == 8);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 8) == 8);
		REQUIRE(*(unsigned*)a3d_vector_get(&vector, 9) == 8);
		a3d_vector_free(&vector);
	}
}
