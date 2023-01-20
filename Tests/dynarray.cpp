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
#include <utility>
#include "Container/dynarray.h"

static unsigned fill_counter = 0;
static unsigned check_counter = 0;

static void fill(const a3d_dynarray_t* dynarray)
{
	for (unsigned* p = (unsigned*)a3d_dynarray_begin(dynarray);
		 p < (unsigned*)a3d_dynarray_end(dynarray);
		 ++p, ++fill_counter)
		 *p = fill_counter;
}

static void check(const a3d_dynarray_t* dynarray)
{
	unsigned i = 0;
	for (unsigned* p = (unsigned*)a3d_dynarray_begin(dynarray);
		 p < (unsigned*)a3d_dynarray_end(dynarray);
		 ++p, ++check_counter)
		 REQUIRE(*p == check_counter);
}

TEST_SUITE("Dynamic Array")
{
	TEST_CASE("Create")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 10);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Create 0")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 0);
		REQUIRE(dynarray.data == nullptr);
		REQUIRE(dynarray.count == 0);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Resize More")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 10);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_resize(&dynarray, 20);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 20);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Resize Less")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 32);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 32);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_resize(&dynarray, 12);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 12);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Resize From 0")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 0);
		REQUIRE(dynarray.data == nullptr);
		REQUIRE(dynarray.count == 0);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_resize(&dynarray, 22);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 22);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Resize To 0")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 10);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_resize(&dynarray, 0);
		REQUIRE(dynarray.data == nullptr);
		REQUIRE(dynarray.count == 0);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("Resize To 0 And Back")
	{
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 10);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_resize(&dynarray, 0);
		REQUIRE(dynarray.data == nullptr);
		REQUIRE(dynarray.count == 0);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		a3d_dynarray_resize(&dynarray, 20);
		REQUIRE(dynarray.data != nullptr);
		REQUIRE(dynarray.count == 20);
		REQUIRE(dynarray.item_size == sizeof(unsigned));
		fill(&dynarray);
		check(&dynarray);
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("First Redundant")
	{
		fill_counter = 0;
		check_counter = 0;
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		fill(&dynarray);
		REQUIRE(*(unsigned*)a3d_dynarray_first_redundant(&dynarray, 5) == 6);
		REQUIRE((unsigned*)a3d_dynarray_first_redundant(&dynarray, 15) == a3d_dynarray_end(&dynarray));
		a3d_dynarray_free(&dynarray);
	}

	TEST_CASE("First Insufficient")
	{
		fill_counter = 0;
		check_counter = 0;
		a3d_dynarray_t dynarray;
		a3d_dynarray_new(&dynarray, sizeof(unsigned), 10);
		fill(&dynarray);
		REQUIRE(*(unsigned*)a3d_dynarray_first_insufficient(&dynarray, 5) == 6);
		REQUIRE((unsigned*)a3d_dynarray_first_insufficient(&dynarray, 15) == a3d_dynarray_end(&dynarray));
		a3d_dynarray_free(&dynarray);
	}
}
