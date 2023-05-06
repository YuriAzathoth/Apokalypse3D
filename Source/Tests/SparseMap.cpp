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
#include <doctest/doctest.h>
#include "Container/sparse_map.h"
#include "DebugAllocator.inl"

static constexpr uint8_t TEST_NUMBERS[] = { 0x55, 0x33, 0x77, 0xF, 0x66 };

using sparse_map = A3D::sparse_map<uint8_t, uint8_t, uint32_t, DebugAllocator<uint8_t>>;

TEST_SUITE("Sparse Map")
{
	TEST_CASE("Idle")
	{{
		sparse_map sm;
		REQUIRE(sm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 1")
	{{
		sparse_map sm;
		int key = sm.insert(TEST_NUMBERS[0]);
		REQUIRE(sm.size() == 1);
		REQUIRE(sm.contains(key));
		REQUIRE(sm[key] == TEST_NUMBERS[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Remove 1")
	{{
		sparse_map sm;
		int key = sm.insert(TEST_NUMBERS[0]);
		REQUIRE(sm.size() == 1);
		REQUIRE(sm.contains(key));
		REQUIRE(sm[key] == TEST_NUMBERS[0]);

		sm.erase(key);
		REQUIRE(sm.size() == 0);
		REQUIRE(!sm.contains(key));
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 5")
	{{
		sparse_map sm;
		int keys[5];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		keys[1] = sm.insert(TEST_NUMBERS[1]);
		keys[2] = sm.insert(TEST_NUMBERS[2]);
		keys[3] = sm.insert(TEST_NUMBERS[3]);
		keys[4] = sm.insert(TEST_NUMBERS[4]);
		REQUIRE(sm.size() == 5);
		REQUIRE(sm[keys[0]] == TEST_NUMBERS[0]);
		REQUIRE(sm[keys[1]] == TEST_NUMBERS[1]);
		REQUIRE(sm[keys[2]] == TEST_NUMBERS[2]);
		REQUIRE(sm[keys[3]] == TEST_NUMBERS[3]);
		REQUIRE(sm[keys[4]] == TEST_NUMBERS[4]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Remove 5")
	{{
		sparse_map sm;
		int keys[5];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		keys[1] = sm.insert(TEST_NUMBERS[1]);
		keys[2] = sm.insert(TEST_NUMBERS[2]);
		keys[3] = sm.insert(TEST_NUMBERS[3]);
		keys[4] = sm.insert(TEST_NUMBERS[4]);
		REQUIRE(sm.size() == 5);

		sm.erase(keys[4]);
		REQUIRE(sm.size() == 4);
		REQUIRE(sm[keys[0]] == TEST_NUMBERS[0]);
		REQUIRE(sm[keys[1]] == TEST_NUMBERS[1]);
		REQUIRE(sm[keys[2]] == TEST_NUMBERS[2]);
		REQUIRE(sm[keys[3]] == TEST_NUMBERS[3]);

		sm.erase(keys[0]);
		REQUIRE(sm.size() == 3);
		REQUIRE(sm[keys[1]] == TEST_NUMBERS[1]);
		REQUIRE(sm[keys[2]] == TEST_NUMBERS[2]);
		REQUIRE(sm[keys[3]] == TEST_NUMBERS[3]);

		sm.erase(keys[2]);
		REQUIRE(sm.size() == 2);
		REQUIRE(sm[keys[1]] == TEST_NUMBERS[1]);
		REQUIRE(sm[keys[3]] == TEST_NUMBERS[3]);

		sm.erase(keys[3]);
		REQUIRE(sm.size() == 1);
		REQUIRE(sm[keys[1]] == TEST_NUMBERS[1]);

		sm.erase(keys[1]);
		REQUIRE(sm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 20")
	{{
		sparse_map sm;
		int keys[20];
		for (unsigned i = 0; i < 20; ++i)
		{
			REQUIRE(!sm.contains(i));
			keys[i] = sm.insert(i);
			REQUIRE(keys[i] == i);
		}

		REQUIRE(sm.size() == 20);

		for (unsigned i = 0; i < 20; ++i)
		{
			REQUIRE(i == keys[i]);
			REQUIRE(i == sm[keys[i]]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 50")
	{{
		sparse_map sm;
		int keys[50];
		for (unsigned i = 0; i < 50; ++i)
		{
			REQUIRE(!sm.contains(i));
			keys[i] = sm.insert(i);
			REQUIRE(keys[i] == i);
		}

		REQUIRE(sm.size() == 50);

		for (unsigned i = 0; i < 50; ++i)
		{
			REQUIRE(i == keys[i]);
			REQUIRE(i == sm[keys[i]]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 1 Utilize 1")
	{{
		sparse_map sm;
		int keys[2];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		REQUIRE(sm.size() == 1);

		sm.erase(keys[0]);
		REQUIRE(sm.size() == 0);

		keys[1] = sm.insert(TEST_NUMBERS[1]);
		REQUIRE(keys[0] == keys[1]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 2 Utilize 1 First")
	{{
		sparse_map sm;
		int keys[3];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		keys[1] = sm.insert(TEST_NUMBERS[1]);
		REQUIRE(sm.size() == 2);

		sm.erase(keys[0]);
		REQUIRE(sm.size() == 1);

		keys[2] = sm.insert(TEST_NUMBERS[2]);
		REQUIRE(sm.size() == 2);
		REQUIRE(keys[2] == keys[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 2 Utilize 1 Last")
	{{
		sparse_map sm;
		int keys[3];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		keys[1] = sm.insert(TEST_NUMBERS[1]);
		REQUIRE(sm.size() == 2);

		sm.erase(keys[1]);
		REQUIRE(sm.size() == 1);

		keys[2] = sm.insert(TEST_NUMBERS[2]);
		REQUIRE(sm.size() == 2);
		REQUIRE(keys[2] == keys[1]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Insert 2 Utilize 2")
	{{
		sparse_map sm;
		int keys[4];
		keys[0] = sm.insert(TEST_NUMBERS[0]);
		keys[1] = sm.insert(TEST_NUMBERS[1]);
		REQUIRE(sm.size() == 2);

		sm.erase(keys[0]);
		sm.erase(keys[1]);
		REQUIRE(sm.size() == 0);

		keys[2] = sm.insert(TEST_NUMBERS[2]);
		keys[3] = sm.insert(TEST_NUMBERS[3]);
		REQUIRE(keys[2] == keys[0]);
		REQUIRE(keys[3] == keys[1]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Clear")
	{{
		sparse_map sm;
		for (unsigned i = 0; i < 5; ++i)
			sm.insert(TEST_NUMBERS[i]);
		REQUIRE(sm.size() == 5);

		sm.clear();
		REQUIRE(sm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy constructor")
	{{
		uint8_t keys[5];

		sparse_map sm1;
		for (unsigned i = 0; i < 5; ++i)
			keys[i] = sm1.insert(TEST_NUMBERS[i]);
		REQUIRE(sm1.size() == 5);

		sparse_map sm2(sm1);
		REQUIRE(sm1.size() == 5);
		REQUIRE(sm2.size() == 5);
		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(sm1[keys[i]] == TEST_NUMBERS[i]);
			REQUIRE(sm2[keys[i]] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		uint8_t keys[5];

		sparse_map sm1;
		for (unsigned i = 0; i < 5; ++i)
			keys[i] = sm1.insert(TEST_NUMBERS[i]);
		REQUIRE(sm1.size() == 5);

		sparse_map sm2(std::move(sm1));
		REQUIRE(sm1.size() == 0);
		REQUIRE(sm2.size() == 5);
		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(sm2[keys[i]] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy assign")
	{{
		uint8_t keys[5];

		sparse_map sm1;
		for (unsigned i = 0; i < 5; ++i)
			keys[i] = sm1.insert(TEST_NUMBERS[i]);
		REQUIRE(sm1.size() == 5);

		sparse_map sm2;
		sm2 = sm1;
		REQUIRE(sm1.size() == 5);
		REQUIRE(sm2.size() == 5);
		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(sm1[keys[i]] == TEST_NUMBERS[i]);
			REQUIRE(sm2[keys[i]] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move assign")
	{{
		uint8_t keys[5];

		sparse_map sm1;
		for (unsigned i = 0; i < 5; ++i)
			keys[i] = sm1.insert(TEST_NUMBERS[i]);
		REQUIRE(sm1.size() == 5);

		sparse_map sm2;
		sm2 = std::move(sm1);
		REQUIRE(sm1.size() == 0);
		REQUIRE(sm2.size() == 5);
		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(sm2[keys[i]] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }
}
