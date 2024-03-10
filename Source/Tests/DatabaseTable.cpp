/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2024 Yuriy Zinchenko

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
		using table_t = A3D::db::data_table::table<A3D::meta::types_list_builder<>::type, uint32_t>;
		table_t table;
	}

	TEST_CASE("Idle non-empty columns")
	{
		using table_t = A3D::db::data_table::table<A3D::meta::types_list_builder<int, float>::type, uint32_t>;
		table_t table;
	}

	TEST_CASE("Table memory size by rows count empty columns")
	{
		using table_t = A3D::db::data_table::table<A3D::meta::types_list_builder<>::type, uint32_t>;
		static_assert(table_t::row_sizeof == 0);
	}

	TEST_CASE("Table memory size by rows count non-empty columns")
	{
		using table_t = A3D::db::data_table::table<A3D::meta::types_list_builder<int, float>::type, uint32_t>;
		static_assert(table_t::row_sizeof == sizeof(int) + sizeof(float));
	}

	TEST_CASE("Memory allocation")
	{
		static constexpr size_t ITEMS_COUNT = 1;
		using types_t = A3D::meta::types_list_builder<int, float>::type;
		using table_t = A3D::db::data_table::table<types_t, uint32_t>;
		table_t table;
		constexpr size_t size = table_t::memory_size(ITEMS_COUNT);
		uint8_t mem[size]; // Allocation in stack.
		table.allocate(mem, ITEMS_COUNT);
	}

	TEST_CASE("Front")
	{
		static constexpr size_t ITEMS_COUNT = 1;
		using types_t = A3D::meta::types_list_builder<int, float>::type;
		using table_t = A3D::db::data_table::table<types_t, uint32_t>;
		table_t table;
		const size_t size = table_t::memory_size(ITEMS_COUNT);
		uint8_t* mem = (uint8_t*)malloc(size);
		table.allocate(mem, ITEMS_COUNT);
		table.create_n(ITEMS_COUNT);
		table.front<int, types_t>() = 5;
		table.front<float, types_t>() = 0.1f;
		REQUIRE(table.front<int, types_t>() == 5);
		REQUIRE(table.front<float, types_t>() == 0.1f);
		table.destroy_n(ITEMS_COUNT);
		free(mem);
	}

	TEST_CASE("Iterator")
	{
		static constexpr size_t ITEMS_COUNT = 2;
		using types_t = A3D::meta::types_list_builder<int, float>::type;
		using table_t = A3D::db::data_table::table<types_t, uint32_t>;
		table_t table;
		const size_t size = table_t::memory_size(ITEMS_COUNT);
		uint8_t* mem = (uint8_t*)malloc(size);
		table.allocate(mem, ITEMS_COUNT);
		table.create_n(ITEMS_COUNT);
		auto it = table.abegin<types_t>();
		it.get<int>() = 10;
		it.get<float>() = 0.5f;
		++it;
		it.get<int>() = 20;
		it.get<float>() = 0.25f;
		REQUIRE(table.front<int, types_t>() == 10);
		REQUIRE(table.front<float, types_t>() == 0.5f);
		REQUIRE(table.at<int, types_t>(1) == 20);
		REQUIRE(table.at<float, types_t>(1) == 0.25f);
		table.destroy_n(ITEMS_COUNT);
		free(mem);
	}

	TEST_CASE("Create 10")
	{
		static constexpr size_t ITEMS_COUNT = 10;
		using types_t = A3D::meta::types_list_builder<int, float>::type;
		using table_t = A3D::db::data_table::table<types_t, uint32_t>;
		table_t table;
		const size_t size = table_t::memory_size(ITEMS_COUNT);
		uint8_t* mem = (uint8_t*)malloc(size);
		table.allocate(mem, ITEMS_COUNT);
		table.create_n(ITEMS_COUNT);
		unsigned i = 0;
		for (i = 0; i < 10; ++i)
		{
			table.at<int, types_t>(i) = i * 10;
			table.at<float, types_t>(i) = i * 0.5f;
		}
		i = 0;
		for (auto it = table.abegin<types_t>(); it != table.amid<types_t>(ITEMS_COUNT); ++it)
		{
			REQUIRE(it.get<int>() == i * 10);
			REQUIRE(it.get<float>() == i * 0.5f);
			++i;
		}
		REQUIRE(i == ITEMS_COUNT);
		table.destroy_n(ITEMS_COUNT);
		free(mem);
	}

	TEST_CASE("Move")
	{
		static constexpr size_t ITEMS_COUNT = 2;
		using types_t = A3D::meta::types_list_builder<int, float>::type;
		using table_t = A3D::db::data_table::table<types_t, uint32_t>;
		table_t table;
		const size_t size = table_t::memory_size(ITEMS_COUNT);
		uint8_t* mem = (uint8_t*)malloc(size);
		table.allocate(mem, ITEMS_COUNT);
		table.create_n(ITEMS_COUNT);
		table.at<int, types_t>(0) = 1;
		table.at<float, types_t>(0) = 0.1f;
		table.at<int, types_t>(1) = 2;
		table.at<float, types_t>(1) = 0.2f;
		table.move<types_t>(table.amid<types_t>(0), table.amid<types_t>(1));
		REQUIRE(table.front<int, types_t>() == 2);
		REQUIRE(table.front<float, types_t>() == 0.2f);
		table.destroy_n(1);
		free(mem);
	}
}
