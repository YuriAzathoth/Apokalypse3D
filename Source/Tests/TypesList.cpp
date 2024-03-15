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
#include "Container/meta/types_list.h"

TEST_SUITE("Types List")
{
	TEST_CASE("Empty Types list")
	{
		using tl_t = A3D::meta::types_list<>;
		static_assert(std::is_same_v<tl_t::value_type, void>);
		static_assert(std::is_same_v<tl_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tl_t::is_empty, A3D::meta::true_type>);
	}

	TEST_CASE("Types list with size 1")
	{
		using tl_t = A3D::meta::types_list<int>;
		static_assert(std::is_same_v<tl_t::value_type, int>);
		static_assert(std::is_same_v<tl_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tl_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Types list with size 2")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		static_assert(std::is_same_v<tl_t::value_type, int>);
		static_assert(std::is_same_v<tl_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tl_t::is_empty, A3D::meta::false_type>);

		using tl2_t = tl_t::next_type;
		static_assert(std::is_same_v<tl2_t::value_type, float>);
		static_assert(std::is_same_v<tl2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tl2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Types list with size 3")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		static_assert(std::is_same_v<tl_t::value_type, int>);
		static_assert(std::is_same_v<tl_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tl_t::is_empty, A3D::meta::false_type>);

		using tl2_t = tl_t::next_type;
		static_assert(std::is_same_v<tl2_t::value_type, float>);
		static_assert(std::is_same_v<tl2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tl2_t::is_empty, A3D::meta::false_type>);

		using tl3_t = tl2_t::next_type;
		static_assert(std::is_same_v<tl3_t::value_type, char>);
		static_assert(std::is_same_v<tl3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tl3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Modify empty")
	{
		using tl_t = A3D::meta::types_list<>;
		using tlm_t = A3D::meta::types_list_modify<std::add_pointer, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, void>);
		static_assert(std::is_same_v<tlm_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm_t::is_empty, A3D::meta::true_type>);
	}

	TEST_CASE("Modify size 1")
	{
		using tl_t = A3D::meta::types_list<int>;
		using tlm_t = A3D::meta::types_list_modify<std::add_pointer, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Modify size 2")
	{
		using tl_t = A3D::meta::types_list<int, float>;

		using tlm1_t = A3D::meta::types_list_modify<std::add_pointer, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Modify size 3")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;

		using tlm1_t = A3D::meta::types_list_modify<std::add_pointer, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);

		using tlm3_t = tlm2_t::next_type;
		static_assert(std::is_same_v<tlm3_t::value_type, char*>);
		static_assert(std::is_same_v<tlm3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Sizeof empty")
	{
		using tl_t = A3D::meta::types_list<>;
		static_assert(A3D::meta::types_list_sizeof<tl_t>::value == 0);
	}

	TEST_CASE("Sizeof size 1")
	{
		using tl_t = A3D::meta::types_list<int>;
		static_assert(A3D::meta::types_list_sizeof<tl_t>::value == sizeof(int));
	}

	TEST_CASE("Sizeof size 2")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		static_assert(A3D::meta::types_list_sizeof<tl_t>::value == sizeof(int) + sizeof(float));
	}

	TEST_CASE("Sizeof size 3")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		static_assert(A3D::meta::types_list_sizeof<tl_t>::value == sizeof(int) + sizeof(float) + sizeof(char));
	}

	TEST_CASE("Type by tag size 1")
	{
		using tl_t = A3D::meta::types_list<int>;
		using tags_t = A3D::meta::types_list<char>;
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<char, tags_t, tl_t>::type, int>);
	}

	TEST_CASE("Type by tag size 2")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		using tags_t = A3D::meta::types_list<char, long>;
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<char, tags_t, tl_t>::type, int>);
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<long, tags_t, tl_t>::type, float>);
	}

	TEST_CASE("Type by tag size 3")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using tags_t = A3D::meta::types_list<char, long, double>;
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<char, tags_t, tl_t>::type, int>);
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<long, tags_t, tl_t>::type, float>);
		static_assert(std::is_same_v<A3D::meta::types_list_type_by_tag<double, tags_t, tl_t>::type, char>);
	}

	TEST_CASE("Cast by tags empty")
	{
		using tl_t = A3D::meta::types_list<>;
		using ttags_t = A3D::meta::types_list<>;
		using stags_t = A3D::meta::types_list<>;
		using tlm_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, void>);
		static_assert(std::is_same_v<tlm_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm_t::is_empty, A3D::meta::true_type>);
	}

	TEST_CASE("Cast by tags size 1")
	{
		using tl_t = A3D::meta::types_list<int>;
		using ttags_t = A3D::meta::types_list<char>;
		using stags_t = ttags_t;
		using tlm_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 2 ordered")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = ttags_t;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 2 reversed")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long, char>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, float*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, int*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 2 same 1")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<char, char>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, int*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 2 same 2")
	{
		using tl_t = A3D::meta::types_list<int, float>;
		using ttags_t = A3D::meta::types_list<char, long>;
		using stags_t = A3D::meta::types_list<long, long>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, float*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 ordered")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = ttags_t;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);

		using tlm3_t = tlm2_t::next_type;
		static_assert(std::is_same_v<tlm3_t::value_type, char*>);
		static_assert(std::is_same_v<tlm3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 reversed")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<double, long, char>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, char*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);

		using tlm3_t = tlm2_t::next_type;
		static_assert(std::is_same_v<tlm3_t::value_type, int*>);
		static_assert(std::is_same_v<tlm3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 shufled 231")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<long, double, char>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, float*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, char*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);

		using tlm3_t = tlm2_t::next_type;
		static_assert(std::is_same_v<tlm3_t::value_type, int*>);
		static_assert(std::is_same_v<tlm3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 shufled 132")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<char, double, long>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, char*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);

		using tlm3_t = tlm2_t::next_type;
		static_assert(std::is_same_v<tlm3_t::value_type, float*>);
		static_assert(std::is_same_v<tlm3_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm3_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 repeated 1")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<char, char>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, int*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, int*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 repeated 2")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<long, long>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, float*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, float*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}

	TEST_CASE("Cast by tags size 3 repeated 3")
	{
		using tl_t = A3D::meta::types_list<int, float, char>;
		using ttags_t = A3D::meta::types_list<char, long, double>;
		using stags_t = A3D::meta::types_list<double, double>;

		using tlm1_t = A3D::meta::types_list_modify_by_tags<std::add_pointer, stags_t, ttags_t, tl_t>::type;
		static_assert(std::is_same_v<tlm1_t::value_type, char*>);
		static_assert(std::is_same_v<tlm1_t::is_last, A3D::meta::false_type>);
		static_assert(std::is_same_v<tlm1_t::is_empty, A3D::meta::false_type>);

		using tlm2_t = tlm1_t::next_type;
		static_assert(std::is_same_v<tlm2_t::value_type, char*>);
		static_assert(std::is_same_v<tlm2_t::is_last, A3D::meta::true_type>);
		static_assert(std::is_same_v<tlm2_t::is_empty, A3D::meta::false_type>);
	}
}
