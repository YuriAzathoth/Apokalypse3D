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
#include <doctest/doctest.h>
#include "Container/meta/data_list.h"

struct test_unary
{
	template <typename T>
	void operator()(T& val)
	{
		++val;
	}
};

struct test_binary
{
	template <typename T, typename U>
	void operator()(T& lhs, const U& rhs)
	{
		lhs += rhs;
	}
};

struct test_ternary
{
	template <typename T, typename U, typename V>
	void operator()(T& res, const U& lhs, const V& rhs)
	{
		res += lhs + rhs;
	}
};

TEST_SUITE("Data List")
{
	TEST_CASE("Empty List")
	{
		using dl_t = A3D::meta::data_list<>;
		static_assert(std::is_same_v<dl_t::value_type, void>);
		static_assert(std::is_same_v<dl_t::types, A3D::meta::types_list<>>);
		static_assert(sizeof(dl_t) == 1);
		dl_t dl;
	}

	TEST_CASE("List with size 1")
	{
		using dl_t = A3D::meta::data_list<int>;
		static_assert(std::is_same_v<dl_t::value_type, int>);
		dl_t dl;
	}

	TEST_CASE("List with size 2")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		static_assert(std::is_same_v<dl_t::types, A3D::meta::types_list<int, float>>);
		dl_t dl;
	}

	TEST_CASE("List with size 3")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		static_assert(std::is_same_v<dl_t::types, A3D::meta::types_list<int, float, char>>);
		dl_t dl;
	}

	TEST_CASE("Get from size 1")
	{
		using dl_t = A3D::meta::data_list<int>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 50;
		REQUIRE(dl.value == 50);
	}

	TEST_CASE("Get from size 2")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 50;
		A3D::meta::data_list_get<float>(dl) = 0.8f;
		REQUIRE(dl.value == 50);
		REQUIRE(dl.next.value == 0.8f);
	}

	TEST_CASE("Get from size 3")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 50;
		A3D::meta::data_list_get<float>(dl) = 0.8f;
		A3D::meta::data_list_get<char>(dl) = 't';
		REQUIRE(dl.value == 50);
		REQUIRE(dl.next.value == 0.8f);
		REQUIRE(dl.next.next.value == 't');
	}

	TEST_CASE("For each unary empty")
	{
		using dl_t = A3D::meta::data_list<>;
		dl_t dl;
		A3D::meta::data_list_foreach(dl, test_unary{});
	}

	TEST_CASE("For each unary size 1")
	{
		using dl_t = A3D::meta::data_list<int>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_foreach(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
	}

	TEST_CASE("For each unary size 2")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
	}

	TEST_CASE("For each unary size 3")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'b');
	}

	TEST_CASE("For each binary empty")
	{
		using dl1_t = A3D::meta::data_list<>;
		using dl2_t = A3D::meta::data_list<>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_foreach(dl1, dl2, test_binary{});
	}

	TEST_CASE("For each binary size 1")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<short>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_foreach(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
	}

	TEST_CASE("For each binary size 2")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary size 3")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'c');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each ternary empty")
	{
		using dl1_t = A3D::meta::data_list<>;
		using dl2_t = A3D::meta::data_list<>;
		using dl3_t = A3D::meta::data_list<>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_foreach(dl1, dl2, dl3, test_ternary{});
	}

	TEST_CASE("For each ternary size 1")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<int>;
		using dl3_t = A3D::meta::data_list<short>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_foreach(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
	}

	TEST_CASE("For each ternary size 2")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_foreach(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
	}

	TEST_CASE("For each ternary size 3")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'h');
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("Get by tag from size 1")
	{
		using dl_t = A3D::meta::data_list<int>;
		using tags_t = A3D::meta::types_list<char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 12;
		auto r = A3D::meta::data_list_get_by_tag<char, tags_t>(dl);
		static_assert(std::is_same_v<decltype(r), int>);
		REQUIRE(r == 12);
	}

	TEST_CASE("Get by tag from size 2")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using tags_t = A3D::meta::types_list<char, long>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 12;
		A3D::meta::data_list_get<float>(dl) = 0.8f;
		auto r1 = A3D::meta::data_list_get_by_tag<char, tags_t>(dl);
		auto r2 = A3D::meta::data_list_get_by_tag<long, tags_t>(dl);
		static_assert(std::is_same_v<decltype(r1), int>);
		static_assert(std::is_same_v<decltype(r2), float>);
		REQUIRE(r1 == 12);
		REQUIRE(r2 == 0.8f);
	}

	TEST_CASE("Get by tag from size 3")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using tags_t = A3D::meta::types_list<char, long, double>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 12;
		A3D::meta::data_list_get<float>(dl) = 0.8f;
		A3D::meta::data_list_get<char>(dl) = 'f';
		auto r1 = A3D::meta::data_list_get_by_tag<char, tags_t>(dl);
		auto r2 = A3D::meta::data_list_get_by_tag<long, tags_t>(dl);
		auto r3 = A3D::meta::data_list_get_by_tag<double, tags_t>(dl);
		static_assert(std::is_same_v<decltype(r1), int>);
		static_assert(std::is_same_v<decltype(r2), float>);
		static_assert(std::is_same_v<decltype(r3), char>);
		REQUIRE(r1 == 12);
		REQUIRE(r2 == 0.8f);
		REQUIRE(r3 == 'f');
	}

	TEST_CASE("For each unary by tags empty")
	{
		using dl_t = A3D::meta::data_list<>;
		using ttags_t = A3D::meta::types_list<>;
		using stags_t = A3D::meta::types_list<>;
		dl_t dl;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
	}

	TEST_CASE("For each unary by tags size 1")
	{
		using dl_t = A3D::meta::data_list<int>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
	}

	TEST_CASE("For each unary by tags size 1 non-unique")
	{
		using dl_t = A3D::meta::data_list<int>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char, char, char, char, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 10);
	}

	TEST_CASE("For each unary by tags size 2 first")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 2.5f);
	}

	TEST_CASE("For each unary by tags size 2 last")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 5);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
	}

	TEST_CASE("For each unary by tags size 2 all ordered")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char, long>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
	}

	TEST_CASE("For each unary by tags size 2 all reversed")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
	}

	TEST_CASE("For each unary by tags size 2 non-unique")
	{
		using dl_t = A3D::meta::data_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char, char, char, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 9);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 2.5f);
	}

	TEST_CASE("For each unary by tags size 3 first")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 2.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'a');
	}

	TEST_CASE("For each unary by tags size 3 mid")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<long>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 5);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'a');
	}

	TEST_CASE("For each unary by tags size 3 last")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<double>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 5);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 2.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'b');
	}

	TEST_CASE("For each unary by tags size 3 all ordered")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<char, long, double>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'b');
	}

	TEST_CASE("For each unary by tags size 3 all reversed")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<double, long, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 3.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'b');
	}

	TEST_CASE("For each unary by tags size 3 non-unique")
	{
		using dl_t = A3D::meta::data_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<double, double, double, double, char, char>;
		dl_t dl;
		A3D::meta::data_list_get<int>(dl) = 5;
		A3D::meta::data_list_get<float>(dl) = 2.5f;
		A3D::meta::data_list_get<char>(dl) = 'a';
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl, test_unary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl) == 7);
		REQUIRE(A3D::meta::data_list_get<float>(dl) == 2.5f);
		REQUIRE(A3D::meta::data_list_get<char>(dl) == 'e');
	}

	TEST_CASE("For each binary by tags empty")
	{
		using dl1_t = A3D::meta::data_list<>;
		using dl2_t = A3D::meta::data_list<>;
		using ttags_t = A3D::meta::types_list<>;
		using stags_t = A3D::meta::types_list<>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
	}

	TEST_CASE("For each binary by tags size 1")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<short>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<short>(dl2) = 5;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 25);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 5);
	}

	TEST_CASE("For each binary by tags size 1 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<short>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char, char, char, char, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<short>(dl2) = 5;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 45);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 5);
	}

	TEST_CASE("For each binary by tags size 2 first")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, float>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 1.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary by tags size 2 last")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, float>;
		using stags_t = A3D::meta::types_list<float>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 20);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary by tags size 2 all ordered")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, float>;
		using stags_t = A3D::meta::types_list<char, float>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary by tags size 2 all reversed")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, float>;
		using stags_t = A3D::meta::types_list<float, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary by tags size 2 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, float>;
		using stags_t = A3D::meta::types_list<char, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 36);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 1.0);
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
	}

	TEST_CASE("For each binary by tags size 3 first")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 1.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'a');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each binary by tags size 3 mid")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<float>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 20);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'a');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each binary by tags size 3 last")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<unsigned>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 20);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 1.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'c');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each binary by tags size 3 all ordered")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<char, float, unsigned>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'c');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each binary by tags size 3 all reversed")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<unsigned, float, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 1.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'c');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each binary by tags size 3 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<char, float, unsigned>;
		using stags_t = A3D::meta::types_list<float, float, float, float, char>;
		dl1_t dl1;
		dl2_t dl2;
		A3D::meta::data_list_get<int>(dl1) = 20;
		A3D::meta::data_list_get<double>(dl1) = 2.0;
		A3D::meta::data_list_get<char>(dl1) = 'a';
		A3D::meta::data_list_get<short>(dl2) = 8;
		A3D::meta::data_list_get<float>(dl2) = 4.0f;
		A3D::meta::data_list_get<char>(dl2) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, test_binary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 28);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 18.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'a');
		REQUIRE(A3D::meta::data_list_get<short>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 2);
	}

	TEST_CASE("For each ternary by tags empty")
	{
		using dl1_t = A3D::meta::data_list<>;
		using dl2_t = A3D::meta::data_list<>;
		using dl3_t = A3D::meta::data_list<>;
		using ttags_t = A3D::meta::types_list<>;
		using stags_t = A3D::meta::types_list<>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
	}

	TEST_CASE("For each ternary by tags size 1")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<int>;
		using dl3_t = A3D::meta::data_list<short>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
	}

	TEST_CASE("For each ternary by tags size 1 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int>;
		using dl2_t = A3D::meta::data_list<int>;
		using dl3_t = A3D::meta::data_list<short>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = A3D::meta::types_list<char, char, char, char, char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 8;
		A3D::meta::data_list_get<short>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 50);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 8);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 2 first")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 0.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
	}

	TEST_CASE("For each ternary by tags size 2 last")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
	}

	TEST_CASE("For each ternary by tags size 2 all ordered")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char, long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
	}

	TEST_CASE("For each ternary by tags size 2 all reversed")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long, char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
	}

	TEST_CASE("For each ternary by tags size 2 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int, double>;
		using dl2_t = A3D::meta::data_list<int, float>;
		using dl3_t = A3D::meta::data_list<short, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long, long, long, long, long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 5.0f;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 30.0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 5.0f);
	}

	TEST_CASE("For each ternary by tags size 3 first")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = A3D::meta::types_list<long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 0.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 3 mid")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = A3D::meta::types_list<double>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 3 last")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = A3D::meta::types_list<char>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 0);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 0.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'h');
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 3 all ordered")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = ttags_t;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'h');
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 3 all reversed")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = A3D::meta::types_list<char, double, long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 16);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 5.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'h');
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}

	TEST_CASE("For each ternary by tags size 3 non-unique")
	{
		using dl1_t = A3D::meta::data_list<int, double, char>;
		using dl2_t = A3D::meta::data_list<int, float, char>;
		using dl3_t = A3D::meta::data_list<short, float, char>;
		using ttags_t = A3D::meta::types_list<long, double, char>;
		using stags_t = A3D::meta::types_list<char, long, long, long, long>;
		dl1_t dl1;
		dl2_t dl2;
		dl3_t dl3;
		A3D::meta::data_list_get<int>(dl1) = 0;
		A3D::meta::data_list_get<double>(dl1) = 0.0;
		A3D::meta::data_list_get<char>(dl1) = 0;
		A3D::meta::data_list_get<int>(dl2) = 10;
		A3D::meta::data_list_get<float>(dl2) = 1.0f;
		A3D::meta::data_list_get<char>(dl2) = 'f';
		A3D::meta::data_list_get<short>(dl3) = 6;
		A3D::meta::data_list_get<float>(dl3) = 4.0f;
		A3D::meta::data_list_get<char>(dl3) = 2;
		A3D::meta::data_list_foreach_by_tags<stags_t, ttags_t>(dl1, dl2, dl3, test_ternary{});
		REQUIRE(A3D::meta::data_list_get<int>(dl1) == 64);
		REQUIRE(A3D::meta::data_list_get<double>(dl1) == 0.0);
		REQUIRE(A3D::meta::data_list_get<char>(dl1) == 'h');
		REQUIRE(A3D::meta::data_list_get<int>(dl2) == 10);
		REQUIRE(A3D::meta::data_list_get<float>(dl2) == 1.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl2) == 'f');
		REQUIRE(A3D::meta::data_list_get<short>(dl3) == 6);
		REQUIRE(A3D::meta::data_list_get<float>(dl3) == 4.0f);
		REQUIRE(A3D::meta::data_list_get<char>(dl3) == 2);
	}
}
