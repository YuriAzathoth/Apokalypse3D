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
#include <string>
#include <doctest/doctest.h>
#include "Container/dense_map.h"
#include "DebugAllocator.inl"

static constexpr uint8_t TEST_NUMBERS[] = { 0x55, 0x33, 0x77, 0xF, 0x66 };
static constexpr const char* TEST_STRINGS[] = { "Alpha", "Betha", "Gamma", "Delta", "Etha" };

using no_pod_type = std::basic_string<char, std::char_traits<char>, DebugAllocator<char>>;
using dense_map = A3D::dense_map<uint8_t, no_pod_type, DebugAllocator<no_pod_type>>;
using dense_map_pod = A3D::dense_map<uint8_t, uint8_t, DebugAllocator<uint8_t>>;

TEST_SUITE("Dense Map (POD)")
{
	TEST_CASE("Idle")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 1")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		dm.push_back(TEST_NUMBERS[0]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 1);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_NUMBERS[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 5")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_NUMBERS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 1")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		dm.push_back(TEST_NUMBERS[0]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 1);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_NUMBERS[0]);

		dm.pop_back();
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 5")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_NUMBERS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm[i] == TEST_NUMBERS[i]);

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm.back() == TEST_NUMBERS[4 - i]);
			REQUIRE(dm.empty() == false);
			REQUIRE(dm.size() == 5 - i);
			dm.pop_back();
		}

		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Erase swap")
	{{
		dense_map_pod dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_NUMBERS[4]);

		REQUIRE(dm.erase_swap(2) == dm.size() - 1);
		dm.pop_back();
		REQUIRE(dm[0] == TEST_NUMBERS[0]);
		REQUIRE(dm[1] == TEST_NUMBERS[1]);
		REQUIRE(dm[2] == TEST_NUMBERS[4]);
		REQUIRE(dm[3] == TEST_NUMBERS[3]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Clear")
	{
		dense_map_pod dm;
		dm.push_back(TEST_NUMBERS[0]);
		dm.push_back(TEST_NUMBERS[1]);
		dm.push_back(TEST_NUMBERS[2]);
		dm.push_back(TEST_NUMBERS[3]);
		dm.push_back(TEST_NUMBERS[4]);
		REQUIRE(dm.size() == 5);

		dm.clear();
		REQUIRE(dm.size() == 0);
	}

	TEST_CASE("Copy constructor")
	{{
		dense_map_pod dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map_pod dm2(dm1);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() > 0);
		REQUIRE(dm2.capacity() > 0);
		REQUIRE(dm1.data() != dm2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm1[i] == TEST_NUMBERS[i]);
			REQUIRE(dm2[i] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		dense_map_pod dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map_pod dm2(std::move(dm1));
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() == 0);
		REQUIRE(dm2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm2[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy assignment")
	{{
		dense_map_pod dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map_pod dm2;
		dm2 = dm1;
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() > 0);
		REQUIRE(dm2.capacity() > 0);
		REQUIRE(dm1.data() != dm2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm1[i] == TEST_NUMBERS[i]);
			REQUIRE(dm2[i] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move assignment")
	{{
		dense_map_pod dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_NUMBERS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map_pod dm2;
		dm2 = std::move(dm1);
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() == 0);
		REQUIRE(dm2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm2[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }
}

TEST_SUITE("Dense Map (Non-POD)")
{
	TEST_CASE("Idle")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 1")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		dm.push_back(TEST_STRINGS[0]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 1);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_STRINGS[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 5")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_STRINGS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_STRINGS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 1")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		dm.push_back(TEST_STRINGS[0]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 1);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_STRINGS[0]);

		dm.pop_back();
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 5")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_STRINGS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_STRINGS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm[i] == TEST_STRINGS[i]);

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm.back() == TEST_STRINGS[4 - i]);
			REQUIRE(dm.empty() == false);
			REQUIRE(dm.size() == 5 - i);
			dm.pop_back();
		}

		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Erase swap")
	{{
		dense_map dm;
		REQUIRE(dm.empty() == true);
		REQUIRE(dm.size() == 0);
		REQUIRE(dm.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_STRINGS[i]);
		REQUIRE(dm.empty() == false);
		REQUIRE(dm.size() == 5);
		REQUIRE(dm.capacity() > 0);
		REQUIRE(dm.back() == TEST_STRINGS[4]);

		REQUIRE(dm.erase_swap(2) == dm.size() - 1);
		dm.pop_back();
		REQUIRE(dm[0] == TEST_STRINGS[0]);
		REQUIRE(dm[1] == TEST_STRINGS[1]);
		REQUIRE(dm[2] == TEST_STRINGS[4]);
		REQUIRE(dm[3] == TEST_STRINGS[3]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Clear")
	{
		dense_map dm;
		for (unsigned i = 0; i < 5; ++i)
			dm.push_back(TEST_STRINGS[i]);
		REQUIRE(dm.size() == 5);
		dm.clear();
		REQUIRE(dm.size() == 0);
	}

	TEST_CASE("Copy constructor")
	{{
		dense_map dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_STRINGS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map dm2(dm1);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() > 0);
		REQUIRE(dm2.capacity() > 0);
		REQUIRE(dm1.data() != dm2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm1[i] == TEST_STRINGS[i]);
			REQUIRE(dm2[i] == TEST_STRINGS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		dense_map dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_STRINGS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map dm2(std::move(dm1));
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() == 0);
		REQUIRE(dm2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm2[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy assignment")
	{{
		dense_map dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_STRINGS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map dm2;
		dm2 = dm1;
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() > 0);
		REQUIRE(dm2.capacity() > 0);
		REQUIRE(dm1.data() != dm2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(dm1[i] == TEST_STRINGS[i]);
			REQUIRE(dm2[i] == TEST_STRINGS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move assignment")
	{{
		dense_map dm1;
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			dm1.push_back(TEST_STRINGS[i]);
		REQUIRE(dm1.empty() == false);
		REQUIRE(dm1.size() == 5);
		REQUIRE(dm1.capacity() > 0);

		dense_map dm2;
		dm2 = std::move(dm1);
		REQUIRE(dm1.empty() == true);
		REQUIRE(dm2.empty() == false);
		REQUIRE(dm1.size() == 0);
		REQUIRE(dm2.size() == 5);
		REQUIRE(dm1.capacity() == 0);
		REQUIRE(dm2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(dm2[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }
}
