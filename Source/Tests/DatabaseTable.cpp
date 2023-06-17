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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <string.h>
#include <doctest/doctest.h>
#include "Container/meta/database_table.h"
#include "DebugAllocator.inl"

TEST_SUITE("Database Table")
{
	TEST_CASE("Idle empty columns")
	{
		using table_t = A3D::db::table::table<A3D::meta::types_list_builder<>::type>;
		table_t table;
	}

	TEST_CASE("Idle non-empty columns")
	{
		using table_t = A3D::db::table::table<A3D::meta::types_list_builder<int, float>::type>;
		table_t table;
	}

	TEST_CASE("Table memory size by rows count empty columns")
	{
		using table_t = A3D::db::table::table<A3D::meta::types_list_builder<>::type>;
		static_assert(A3D::db::table::memory_size<table_t>(10) == 0);
	}

	TEST_CASE("Table memory size by rows count non-empty columns")
	{
		using table_t = A3D::db::table::table<A3D::meta::types_list_builder<int, float>::type>;
		static_assert(A3D::db::table::memory_size<table_t>(10) == 10 * (sizeof(int) + sizeof(float)));
	}

	TEST_CASE("Memory allocation")
	{
		using table_t = A3D::db::table::table<A3D::meta::types_list_builder<int, float>::type>;
		table_t table;
		const size_t size = A3D::db::table::memory_size<table_t>(10);
		uint8_t* mem = (uint8_t*)malloc(size);
		A3D::meta::foreach(table, A3D::db::table::distribute_memory{mem, 10});
		A3D::meta::foreach(table, A3D::db::table::construct_data{10});
		for (unsigned i = 0; i < 10; ++i)
		{
			A3D::meta::get<int*>(table)[i] = i * 10;
			A3D::meta::get<float*>(table)[i] = i * 0.5f;
		}
		for (unsigned i = 0; i < 10; ++i)
		{
			REQUIRE(A3D::meta::get<int*>(table)[i] == i * 10);
			REQUIRE(A3D::meta::get<float*>(table)[i] == i * 0.5f);
		}
		A3D::meta::foreach(table, A3D::db::table::destroy_data{10});
		free(mem);
	}
}
