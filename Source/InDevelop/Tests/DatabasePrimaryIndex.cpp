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
#include <type_traits>
#include <doctest/doctest.h>
#include "Container/meta/database_primary_index.h"
#include "DebugAllocator.inl"

TEST_SUITE("Database Primary Index")
{
	TEST_CASE("Idle")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
	}

	TEST_CASE("Empty chunk value")
	{
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint8_t>::chunk_empty == 0u);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint16_t>::chunk_empty == 0u);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::chunk_empty == 0u);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint64_t>::chunk_empty == 0u);
	}

	TEST_CASE("Full chunk value")
	{
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint8_t>::chunk_full == 0xffu);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint16_t>::chunk_full == 0xffffu);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::chunk_full == 0xffffffffu);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint64_t>::chunk_full == 0xffffffffffffffffLLu);
	}

	TEST_CASE("Invalid key value")
	{
		// REQUIRE(A3D::db::primary_index::index<uint8_t, uint32_t>::invalid_key == 0xffu); // Can not narrow 8 bit
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::invalid_key == 0xffffu);
		REQUIRE(A3D::db::primary_index::index<uint32_t, uint32_t>::invalid_key == 0xffffffffu);
		REQUIRE(A3D::db::primary_index::index<uint64_t, uint32_t>::invalid_key == 0xffffffffffffffffLLu);
	}

	TEST_CASE("Chunk size value")
	{
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint8_t>::chunk_size == 8);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint16_t>::chunk_size == 16);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::chunk_size == 32);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint64_t>::chunk_size == 64);
	}

	TEST_CASE("Bits mask value")
	{
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint8_t>::bits_mask == 7);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint16_t>::bits_mask == 15);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::bits_mask == 31);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint64_t>::bits_mask == 63);
	}

	TEST_CASE("Bits shift value")
	{
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint8_t>::bits_shift == 3);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint16_t>::bits_shift == 4);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint32_t>::bits_shift == 5);
		REQUIRE(A3D::db::primary_index::index<uint16_t, uint64_t>::bits_shift == 6);
	}

	TEST_CASE("Get chunk id 16 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint16_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_chunk_id(i * 16) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 16 + 8) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 16 + 15) == i);
		}
	}

	TEST_CASE("Get chunk id 32 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_chunk_id(i * 32) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 32 + 16) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 32 + 31) == i);
		}
	}

	TEST_CASE("Get chunk id 64 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint64_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_chunk_id(i * 64) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 64 + 32) == i);
			REQUIRE(dpi_t::get_chunk_id(i * 64 + 63) == i);
		}
	}

	TEST_CASE("Get bit id 16 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint16_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_bit_id(i * 16) == 0);
			REQUIRE(dpi_t::get_bit_id(i * 16 + 8) == 8);
			REQUIRE(dpi_t::get_bit_id(i * 16 + 15) == 15);
		}
	}

	TEST_CASE("Get bit id 32 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_bit_id(i * 32) == 0);
			REQUIRE(dpi_t::get_bit_id(i * 32 + 16) == 16);
			REQUIRE(dpi_t::get_bit_id(i * 32 + 31) == 31);
		}
	}

	TEST_CASE("Get bit id 64 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint64_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_bit_id(i * 64) == 0);
			REQUIRE(dpi_t::get_bit_id(i * 64 + 32) == 32);
			REQUIRE(dpi_t::get_bit_id(i * 64 + 63) == 63);
		}
	}

	TEST_CASE("Get key 16 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint16_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_key(i, 0) == (i << 4 | 0));
			REQUIRE(dpi_t::get_key(i, 8) == (i << 4 | 8));
			REQUIRE(dpi_t::get_key(i, 15) == (i << 4 | 15));
		}
	}

	TEST_CASE("Get key 32 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_key(i, 0) == (i << 5 | 0));
			REQUIRE(dpi_t::get_key(i, 16) == (i << 5 | 16));
			REQUIRE(dpi_t::get_key(i, 31) == (i << 5 | 31));
		}
	}

	TEST_CASE("Get key 64 bit chunk")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint64_t>;
		for (unsigned i = 0; i < 128; i += 8)
		{
			REQUIRE(dpi_t::get_key(i, 0) == (i << 6 | 0));
			REQUIRE(dpi_t::get_key(i, 32) == (i << 6 | 32));
			REQUIRE(dpi_t::get_key(i, 63) == (i << 6 | 63));
		}
	}

	TEST_CASE("Indices size")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		REQUIRE(dpi_t::indices_size(1) == 1 * sizeof(uint16_t));
		REQUIRE(dpi_t::indices_size(10) == 10 * sizeof(uint16_t));
		REQUIRE(dpi_t::indices_size(50) == 50 * sizeof(uint16_t));
		REQUIRE(dpi_t::indices_size(100) == 100 * sizeof(uint16_t));
	}

	TEST_CASE("States size")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		REQUIRE(dpi_t::states_size(32) == 1 * sizeof(uint32_t));
		REQUIRE(dpi_t::states_size(64) == 2 * sizeof(uint32_t));
		REQUIRE(dpi_t::states_size(96) == 3 * sizeof(uint32_t));
		REQUIRE(dpi_t::states_size(128) == 4 * sizeof(uint32_t));
	}

	TEST_CASE("Insert 1")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi[0] == 10);
		free(mem);
	}

	TEST_CASE("Insert 1 Erase")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi[0] == 10);
		dpi.erase(0);
		REQUIRE(dpi.contains(0) == false);
		free(mem);
	}

	TEST_CASE("Insert 1 Reuse")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi[0] == 10);
		dpi.erase(0);
		REQUIRE(dpi.contains(0) == false);
		REQUIRE(dpi.insert(50, 1) == 0);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi[0] == 50);
		free(mem);
	}

	TEST_CASE("Insert 2")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.insert(20, 1) == 1);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi.contains(1) == true);
		REQUIRE(dpi[0] == 10);
		REQUIRE(dpi[1] == 20);
		free(mem);
	}

	TEST_CASE("Insert 2 Erase 1 First")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.insert(20, 1) == 1);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi.contains(1) == true);
		REQUIRE(dpi[0] == 10);
		REQUIRE(dpi[1] == 20);
		dpi.erase(0);
		REQUIRE(dpi.contains(0) == false);
		REQUIRE(dpi.contains(1) == true);
		free(mem);
	}

	TEST_CASE("Insert 2 Erase 1 Last")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.insert(20, 1) == 1);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi.contains(1) == true);
		REQUIRE(dpi[0] == 10);
		REQUIRE(dpi[1] == 20);
		dpi.erase(1);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi.contains(1) == false);
		free(mem);
	}

	TEST_CASE("Insert 2 Erase 2")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		REQUIRE(dpi.insert(10, 1) == 0);
		REQUIRE(dpi.insert(20, 1) == 1);
		REQUIRE(dpi.contains(0) == true);
		REQUIRE(dpi.contains(1) == true);
		REQUIRE(dpi[0] == 10);
		REQUIRE(dpi[1] == 20);
		dpi.erase(0);
		dpi.erase(1);
		REQUIRE(dpi.contains(0) == false);
		REQUIRE(dpi.contains(1) == false);
		free(mem);
	}

	TEST_CASE("Insert 8")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size) + dpi_t::states_size(dpi_t::chunk_size));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size), dpi_t::states_size(dpi_t::chunk_size));
		dpi.clear(1);
		for (unsigned i = 0; i < 8; ++i)
			REQUIRE(dpi.insert(i * 5, 1) == i);
		for (unsigned i = 0; i < 8; ++i)
			REQUIRE(dpi.contains(i) == true);
		for (unsigned i = 0; i < 8; ++i)
			REQUIRE(dpi[i] == i * 5);
		free(mem);
	}

	TEST_CASE("Insert 48")
	{
		using dpi_t = A3D::db::primary_index::index<uint16_t, uint32_t>;
		dpi_t dpi;
		uint8_t* mem = (uint8_t*)malloc(dpi_t::indices_size(dpi_t::chunk_size * 2) + dpi_t::states_size(dpi_t::chunk_size * 2));
		dpi.allocate(mem, dpi_t::indices_size(dpi_t::chunk_size * 2), dpi_t::states_size(dpi_t::chunk_size * 2));
		dpi.clear(2);
		for (unsigned i = 0; i < 48; ++i)
			REQUIRE(dpi.insert(i * 5, 2) == i);
		for (unsigned i = 0; i < 48; ++i)
			REQUIRE(dpi.contains(i) == true);
		for (unsigned i = 0; i < 48; ++i)
			REQUIRE(dpi[i] == i * 5);
		free(mem);
	}
}
