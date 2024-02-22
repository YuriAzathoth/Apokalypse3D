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
#include "Container/string.h"
#include "DebugAllocator.inl"

static constexpr const char TEST_STRING[] = "Hello, World!";
static constexpr const char TEST_STRING_JUNK[] = "One Two Three Four Five";
static constexpr const char TEST_PART_1[] = "Hello, ";
static constexpr const char TEST_PART_2[] = "World!";

TEST_SUITE("String View")
{
	TEST_CASE("Default constructor")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s;
		REQUIRE(s.empty());
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Const char constructor")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s(TEST_STRING);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Copy constructor")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2(s1);
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Move constructor")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2(std::move(s1));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Const char assignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s;
		s = TEST_STRING;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Copy assignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2;
		s2 = s1;
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Move assignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2;
		s2 = std::move(s1);
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Const char reassignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s(TEST_STRING_JUNK);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING_JUNK));
		s = TEST_STRING;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Copy reassignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2(TEST_STRING_JUNK);
		s2 = s1;
		REQUIRE(!strcmp(s1.c_str(), TEST_STRING));
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Move reassignment")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		A3D::basic_string<char, DebugAllocator<char>> s2(TEST_STRING_JUNK);
		s2 = std::move(s1);
		REQUIRE(!strcmp(s2.c_str(), TEST_STRING));
		REQUIRE(s1.c_str() == nullptr);
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Const char append")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s(TEST_PART_1);
		s += TEST_PART_2;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("String append")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s(TEST_PART_1);
		A3D::basic_string<char, DebugAllocator<char>> a(TEST_PART_2);
		s += a;
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Const char concat")
	{{
		A3D::basic_string<char, DebugAllocator<char>> a(TEST_PART_1);
		A3D::basic_string<char, DebugAllocator<char>> s(a + TEST_PART_2);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("String concat")
	{{
		A3D::basic_string<char, DebugAllocator<char>> a(TEST_PART_1);
		A3D::basic_string<char, DebugAllocator<char>> b(TEST_PART_2);
		A3D::basic_string<char, DebugAllocator<char>> s(a + b);
		REQUIRE(!strcmp(s.c_str(), TEST_STRING));
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Forward iterator")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING;
		for (auto it = s1.begin(); it != s1.end(); ++it)
		{
			REQUIRE(*it == *s2);
			++s2;
		}
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Forward const iterator")
	{{
		const A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING;
		for (auto it = s1.begin(); it != s1.end(); ++it)
		{
			REQUIRE(*it == *s2);
			++s2;
		}
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Reverse iterator")
	{{
		A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING + sizeof(TEST_STRING) - 1;
		for (auto it = s1.rbegin(); it != s1.rend(); ++it)
		{
			REQUIRE(*it == *s2);
			--s2;
		}
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Reverse const iterator")
	{{
		const A3D::basic_string<char, DebugAllocator<char>> s1(TEST_STRING);
		const char* s2 = TEST_STRING + sizeof(TEST_STRING) - 1;
		for (auto it = s1.rbegin(); it != s1.rend(); ++it)
		{
			REQUIRE(*it == *s2);
			--s2;
		}
	}
	CheckMemoryLeaks(); }

	TEST_CASE("Copy-on-write")
	{{
		A3D::basic_string<char, DebugAllocator<char>> a(TEST_STRING);
		REQUIRE(a.refs() == 1);
		A3D::basic_string<char, DebugAllocator<char>> b(a);
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
	CheckMemoryLeaks(); }
}
