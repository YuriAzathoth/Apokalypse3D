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
#include "Container/cpp_lifecycle.h"

#define ARRAY_SIZE 16

static size_t s_default_constructors;
static size_t s_destructors;
static size_t s_copy_constructors;
static size_t s_move_constructors;
static size_t s_copy_assigners;
static size_t s_move_assigners;

static void clean()
{
	s_default_constructors = 0;
	s_destructors = 0;
	s_copy_constructors = 0;
	s_move_constructors = 0;
	s_copy_assigners = 0;
	s_move_assigners = 0;
}

struct test
{
	test() { ++s_default_constructors; }
	~test() { ++s_destructors; }
	test(const test&) { ++s_copy_constructors; }
	test(test&&) noexcept { ++s_move_constructors; }
	void operator=(const test&) { ++s_copy_assigners; }
	void operator=(test&&) noexcept { ++s_move_assigners; }

	int _;
};

TEST_SUITE("Database Table")
{
	TEST_CASE("Default constructor")
	{
		clean();
		uint8_t mem[sizeof(test)];
		test* p = reinterpret_cast<test*>(mem);
		A3D::construct(p);
		REQUIRE(s_default_constructors == 1);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Destructor")
	{
		clean();
		uint8_t mem[sizeof(test)];
		test* p = reinterpret_cast<test*>(mem);
		A3D::destroy(p);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 1);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Copy constructor")
	{
		clean();
		uint8_t mem1[sizeof(test)];
		uint8_t mem2[sizeof(test)];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::copy_construct(p1, p2);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 1);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Move constructor")
	{
		clean();
		uint8_t mem1[sizeof(test)];
		uint8_t mem2[sizeof(test)];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::move_construct(p1, p2);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 1);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Copy assigner")
	{
		clean();
		uint8_t mem1[sizeof(test)];
		uint8_t mem2[sizeof(test)];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::copy_assign(p1, p2);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 1);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Move assigner")
	{
		clean();
		uint8_t mem1[sizeof(test)];
		uint8_t mem2[sizeof(test)];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::move_assign(p1, p2);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 1);
	}

	TEST_CASE("Default constructor range")
	{
		clean();
		uint8_t mem[sizeof(test) * ARRAY_SIZE];
		test* p = reinterpret_cast<test*>(mem);
		A3D::construct_n(p, ARRAY_SIZE);
		REQUIRE(s_default_constructors == ARRAY_SIZE);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Destructor range")
	{
		clean();
		uint8_t mem[sizeof(test) * ARRAY_SIZE];
		test* p = reinterpret_cast<test*>(mem);
		A3D::destroy_n(p, ARRAY_SIZE);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == ARRAY_SIZE);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Copy constructor range")
	{
		clean();
		uint8_t mem1[sizeof(test) * ARRAY_SIZE];
		uint8_t mem2[sizeof(test) * ARRAY_SIZE];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::copy_construct_n(p1, p2, ARRAY_SIZE);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == ARRAY_SIZE);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Move constructor range")
	{
		clean();
		uint8_t mem1[sizeof(test) * ARRAY_SIZE];
		uint8_t mem2[sizeof(test) * ARRAY_SIZE];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::move_construct_n(p1, p2, ARRAY_SIZE);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == ARRAY_SIZE);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Copy assigner range")
	{
		clean();
		uint8_t mem1[sizeof(test) * ARRAY_SIZE];
		uint8_t mem2[sizeof(test) * ARRAY_SIZE];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::copy_assign_n(p1, p2, ARRAY_SIZE);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == ARRAY_SIZE);
		REQUIRE(s_move_assigners == 0);
	}

	TEST_CASE("Move assigner range")
	{
		clean();
		uint8_t mem1[sizeof(test) * ARRAY_SIZE];
		uint8_t mem2[sizeof(test) * ARRAY_SIZE];
		test* p1 = reinterpret_cast<test*>(mem1);
		test* p2 = reinterpret_cast<test*>(mem2);
		A3D::move_assign_n(p1, p2, ARRAY_SIZE);
		REQUIRE(s_default_constructors == 0);
		REQUIRE(s_destructors == 0);
		REQUIRE(s_copy_constructors == 0);
		REQUIRE(s_move_constructors == 0);
		REQUIRE(s_copy_assigners == 0);
		REQUIRE(s_move_assigners == ARRAY_SIZE);
	}
}
