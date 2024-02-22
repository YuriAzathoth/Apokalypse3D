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
#include <string>
#include <doctest/doctest.h>
#include "Container/vector.h"
#include "DebugAllocator.inl"

static constexpr uint8_t TEST_NUMBERS[] = { 0x55, 0x33, 0x77, 0xF, 0x66 };
static constexpr const char* TEST_STRINGS[] = { "Alpha", "Betha", "Gamma", "Delta", "Etha" };

using no_pod_type = std::basic_string<char, std::char_traits<char>, DebugAllocator<char>>;
using vector = A3D::vector<uint8_t, no_pod_type, DebugAllocator<no_pod_type>>;
using vector_pod = A3D::vector<uint8_t, uint8_t, DebugAllocator<uint8_t>>;

TEST_SUITE("Vector (POD)")
{
	TEST_CASE("Idle")
	{{
		vector_pod vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 1")
	{{
		vector_pod vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		vec.push_back(TEST_NUMBERS[0]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 1);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_NUMBERS[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 5")
	{{
		vector_pod vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 5);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_NUMBERS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 1")
	{{
		vector_pod vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		vec.push_back(TEST_NUMBERS[0]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 1);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_NUMBERS[0]);

		vec.pop_back();
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 5")
	{{
		vector_pod vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 5);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_NUMBERS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec[i] == TEST_NUMBERS[i]);

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec.back() == TEST_NUMBERS[4 - i]);
			REQUIRE(vec.empty() == false);
			REQUIRE(vec.size() == 5 - i);
			vec.pop_back();
		}

		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Clear")
	{
		vector_pod vec;
		vec.push_back(TEST_NUMBERS[0]);
		vec.push_back(TEST_NUMBERS[1]);
		vec.push_back(TEST_NUMBERS[2]);
		vec.push_back(TEST_NUMBERS[3]);
		vec.push_back(TEST_NUMBERS[4]);
		REQUIRE(vec.size() == 5);

		vec.clear();
		REQUIRE(vec.size() == 0);
	}

	TEST_CASE("Copy constructor")
	{{
		vector_pod vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector_pod vec2(vec1);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() > 0);
		REQUIRE(vec2.capacity() > 0);
		REQUIRE(vec1.data() != vec2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec1[i] == TEST_NUMBERS[i]);
			REQUIRE(vec2[i] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		vector_pod vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector_pod vec2(std::move(vec1));
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() == 0);
		REQUIRE(vec2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec2[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy assignment")
	{{
		vector_pod vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector_pod vec2;
		vec2 = vec1;
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() > 0);
		REQUIRE(vec2.capacity() > 0);
		REQUIRE(vec1.data() != vec2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec1[i] == TEST_NUMBERS[i]);
			REQUIRE(vec2[i] == TEST_NUMBERS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move assignment")
	{{
		vector_pod vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_NUMBERS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector_pod vec2;
		vec2 = std::move(vec1);
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() == 0);
		REQUIRE(vec2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec2[i] == TEST_NUMBERS[i]);
	} CheckMemoryLeaks(); }
}

TEST_SUITE("Vector (Non-POD)")
{
	TEST_CASE("Idle")
	{{
		vector vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 1")
	{{
		vector vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		vec.push_back(TEST_STRINGS[0]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 1);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_STRINGS[0]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Push back 5")
	{{
		vector vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec.push_back(TEST_STRINGS[i]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 5);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_STRINGS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 1")
	{{
		vector vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		vec.push_back(TEST_STRINGS[0]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 1);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_STRINGS[0]);

		vec.pop_back();
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Pop back 5")
	{{
		vector vec;
		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
		REQUIRE(vec.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec.push_back(TEST_STRINGS[i]);
		REQUIRE(vec.empty() == false);
		REQUIRE(vec.size() == 5);
		REQUIRE(vec.capacity() > 0);
		REQUIRE(vec.back() == TEST_STRINGS[4]);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec[i] == TEST_STRINGS[i]);

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec.back() == TEST_STRINGS[4 - i]);
			REQUIRE(vec.empty() == false);
			REQUIRE(vec.size() == 5 - i);
			vec.pop_back();
		}

		REQUIRE(vec.empty() == true);
		REQUIRE(vec.size() == 0);
	} CheckMemoryLeaks(); }

	TEST_CASE("Clear")
	{
		vector vec;
		for (unsigned i = 0; i < 5; ++i)
			vec.push_back(TEST_STRINGS[i]);
		REQUIRE(vec.size() == 5);
		vec.clear();
		REQUIRE(vec.size() == 0);
	}

	TEST_CASE("Copy constructor")
	{{
		vector vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_STRINGS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector vec2(vec1);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() > 0);
		REQUIRE(vec2.capacity() > 0);
		REQUIRE(vec1.data() != vec2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec1[i] == TEST_STRINGS[i]);
			REQUIRE(vec2[i] == TEST_STRINGS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		vector vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_STRINGS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector vec2(std::move(vec1));
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() == 0);
		REQUIRE(vec2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec2[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }

	TEST_CASE("Copy assignment")
	{{
		vector vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_STRINGS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector vec2;
		vec2 = vec1;
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() > 0);
		REQUIRE(vec2.capacity() > 0);
		REQUIRE(vec1.data() != vec2.data());

		for (unsigned i = 0; i < 5; ++i)
		{
			REQUIRE(vec1[i] == TEST_STRINGS[i]);
			REQUIRE(vec2[i] == TEST_STRINGS[i]);
		}
	} CheckMemoryLeaks(); }

	TEST_CASE("Move assignment")
	{{
		vector vec1;
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec1.capacity() == 0);

		for (unsigned i = 0; i < 5; ++i)
			vec1.push_back(TEST_STRINGS[i]);
		REQUIRE(vec1.empty() == false);
		REQUIRE(vec1.size() == 5);
		REQUIRE(vec1.capacity() > 0);

		vector vec2;
		vec2 = std::move(vec1);
		REQUIRE(vec1.empty() == true);
		REQUIRE(vec2.empty() == false);
		REQUIRE(vec1.size() == 0);
		REQUIRE(vec2.size() == 5);
		REQUIRE(vec1.capacity() == 0);
		REQUIRE(vec2.capacity() > 0);

		for (unsigned i = 0; i < 5; ++i)
			REQUIRE(vec2[i] == TEST_STRINGS[i]);
	} CheckMemoryLeaks(); }
}
