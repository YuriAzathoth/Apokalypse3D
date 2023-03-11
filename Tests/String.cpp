/*
	RECS - Rapid Entity Component System
	Copyright (C) 2023 Yuriy Zinchenko

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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest/doctest.h"
#include "Container/string.h"

std::unordered_map<void*, int> g_Allocs;

template <typename T>
struct test_allocator
{
	using value_type = T;
	using size_type = uint32_t;
	using difference_type = int32_t;

	T* allocate(size_t size)
	{
		T* ret = (T*)malloc(size * sizeof(T));
		g_Allocs.emplace(ret, 1);
		return ret;
	}

	void deallocate(T* ptr, size_t)
	{
		auto it = g_Allocs.find(ptr);
		if (it != g_Allocs.end() && it->second == 1)
		{
			g_Allocs.erase(it);
			free(ptr);
		}
		else
		{
			std::cerr << "FATAL ERROR: Memory leaks detected: Redundant free!" << std::endl;
			std::terminate();
		}
	}
};

void check_memory_leaks()
{
	if (!g_Allocs.empty())
	{
		std::cerr << "FATAL ERROR: Memory leaks detected: Malloc without free!" << std::endl;
		std::terminate();
	}
}

static constexpr const char TEST_STRING[] = "Hello, World!";
static constexpr const char TEST_STRING_JUNK[] = "One Two Three Four Five";
static constexpr const char TEST_PART_1[] = "Hello, ";
static constexpr const char TEST_PART_2[] = "World!";

TEST_SUITE("String View")
{
	TEST_CASE("Default constructor")
	{{
		Recs::string<test_allocator<char>> s;
		REQUIRE(s.empty());
	}
	check_memory_leaks(); }

	TEST_CASE("Const char constructor")
	{{
		Recs::string<test_allocator<char>> s(TEST_STRING);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Copy constructor")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2(s1);
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Move constructor")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2(std::move(s1));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	check_memory_leaks(); }

	TEST_CASE("Const char assignment")
	{{
		Recs::string<test_allocator<char>> s;
		s = TEST_STRING;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Copy assignment")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2;
		s2 = s1;
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Move assignment")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2;
		s2 = std::move(s1);
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	check_memory_leaks(); }

	TEST_CASE("Const char reassignment")
	{{
		Recs::string<test_allocator<char>> s(TEST_STRING_JUNK);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING_JUNK));
		s = TEST_STRING;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Copy reassignment")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2(TEST_STRING_JUNK);
		s2 = s1;
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Move reassignment")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		Recs::string<test_allocator<char>> s2(TEST_STRING_JUNK);
		s2 = std::move(s1);
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	check_memory_leaks(); }

	TEST_CASE("Const char append")
	{{
		Recs::string<test_allocator<char>> s(TEST_PART_1);
		s += TEST_PART_2;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("String append")
	{{
		Recs::string<test_allocator<char>> s(TEST_PART_1);
		Recs::string<test_allocator<char>> a(TEST_PART_2);
		s += a;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Const char concat")
	{{
		Recs::string<test_allocator<char>> a(TEST_PART_1);
		Recs::string<test_allocator<char>> s(a + TEST_PART_2);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("String concat")
	{{
		Recs::string<test_allocator<char>> a(TEST_PART_1);
		Recs::string<test_allocator<char>> b(TEST_PART_2);
		Recs::string<test_allocator<char>> s(a + b);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	check_memory_leaks(); }

	TEST_CASE("Forward iterator")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING;
		for (auto it = s1.begin(); it != s1.end(); ++it)
		{
			REQUIRE(*it == *s2);
			++s2;
		}
	}
	check_memory_leaks(); }

	TEST_CASE("Forward const iterator")
	{{
		const Recs::string<test_allocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING;
		for (auto it = s1.begin(); it != s1.end(); ++it)
		{
			REQUIRE(*it == *s2);
			++s2;
		}
	}
	check_memory_leaks(); }

	TEST_CASE("Reverse iterator")
	{{
		Recs::string<test_allocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING + sizeof(TEST_STRING) - 1;
		for (auto it = s1.rbegin(); it != s1.rend(); ++it)
		{
			REQUIRE(*it == *s2);
			--s2;
		}
	}
	check_memory_leaks(); }

	TEST_CASE("Reverse const iterator")
	{{
		const Recs::string<test_allocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING + sizeof(TEST_STRING) - 1;
		for (auto it = s1.rbegin(); it != s1.rend(); ++it)
		{
			REQUIRE(*it == *s2);
			--s2;
		}
	}
	check_memory_leaks(); }

	TEST_CASE("Copy-on-write")
	{{
		Recs::string<test_allocator<char>> a(TEST_STRING);
		REQUIRE(a.refs() == 1);
		Recs::string<test_allocator<char>> b(a);
		REQUIRE(!strcmp(a.c_str(), TEST_STRING));
		REQUIRE(!strcmp(b.c_str(), TEST_STRING));
		REQUIRE(a.c_str() == b.c_str());
		REQUIRE(a.refs() == 2);
		REQUIRE(b.refs() == 2);
		b = TEST_PART_2;
		REQUIRE(!strcmp(a.c_str(), TEST_STRING));
		REQUIRE(!strcmp(b.c_str(), TEST_PART_2));
		REQUIRE(a.c_str() != b.c_str());
		REQUIRE(a.refs() == 1);
		REQUIRE(b.refs() == 1);
	}
	check_memory_leaks(); }
}
