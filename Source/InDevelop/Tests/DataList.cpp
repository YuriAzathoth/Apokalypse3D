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
#include "Container/meta/types_list.h"

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
		res = lhs + rhs;
	}
};

TEST_SUITE("Data List")
{
	TEST_CASE("Empty List")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		static_assert(std::is_same_v<dl_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<dl_t::next_type, A3D::meta::empty_type>);
		static_assert(sizeof(dl_t) == 1);
	}

	TEST_CASE("List with size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		static_assert(std::is_same_v<dl_t::value_type, int>);
		static_assert(std::is_same_v<dl_t::next_type, A3D::meta::empty_type>);
		static_assert(A3D::meta::list_sizeof<dl_t>() == sizeof(int));
		static_assert(sizeof(dl_t) == sizeof(int));
	}

	TEST_CASE("List with size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		static_assert(std::is_same_v<dl_t::value_type, int>);
		static_assert(std::is_same_v<dl_t::next_type::value_type, float>);
		static_assert(std::is_same_v<dl_t::next_type::next_type, A3D::meta::empty_type>);
		static_assert(A3D::meta::list_sizeof<dl_t>() == sizeof(int) + sizeof(float));
		static_assert(sizeof(dl_t) == sizeof(int) + sizeof(float));
	}

	TEST_CASE("Get from size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		REQUIRE(A3D::meta::get<int>(dl) == 50);
	}

	TEST_CASE("Get from size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		A3D::meta::get<float>(dl) = 2.5f;
		REQUIRE(A3D::meta::get<int>(dl) == 50);
		REQUIRE(A3D::meta::get<float>(dl) == 2.5f);
	}

	TEST_CASE("Modify empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<dlm_t::next_type, A3D::meta::empty_type>);
		static_assert(sizeof(dlm_t) == 1);
	}

	TEST_CASE("Modify size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, int*>);
		static_assert(std::is_same_v<dlm_t::next_type, A3D::meta::empty_type>);
		static_assert(sizeof(dlm_t) == sizeof(int*));
	}

	TEST_CASE("Modify size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, int*>);
		static_assert(std::is_same_v<dlm_t::next_type::value_type, float*>);
		static_assert(std::is_same_v<dlm_t::next_type::next_type, A3D::meta::empty_type>);
		static_assert(sizeof(dlm_t) == sizeof(int*) + sizeof(float*));
	}

	TEST_CASE("Sizeof empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		static_assert(A3D::meta::list_sizeof<dl_t>() == 0);
	}

	TEST_CASE("Sizeof size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		static_assert(A3D::meta::list_sizeof<dl_t>() == sizeof(int));
	}

	TEST_CASE("Sizeof size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		static_assert(A3D::meta::list_sizeof<dl_t>() == sizeof(int) + sizeof(float));
	}

	TEST_CASE("Type by tag size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using tags_t = A3D::meta::data_list_builder<char>::type;
		static_assert(std::is_same_v<A3D::meta::type_by_tag<char, tags_t, dl_t>::type, int>);
	}

	TEST_CASE("Type by tag size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using tags_t = A3D::meta::data_list_builder<char, long>::type;
		static_assert(std::is_same_v<A3D::meta::type_by_tag<char, tags_t, dl_t>::type, int>);
		static_assert(std::is_same_v<A3D::meta::type_by_tag<long, tags_t, dl_t>::type, float>);
	}

	TEST_CASE("Cast by tags empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		using tags_t = A3D::meta::data_list_builder<>::type;
		using dlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::data_list, dl_t, tags_t, tags_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<dlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using tags_t = A3D::meta::data_list_builder<char>::type;
		using dlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::data_list, dl_t, tags_t, tags_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, int*>);
		static_assert(std::is_same_v<dlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 2 ordered")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using tags_t = A3D::meta::data_list_builder<char, long>::type;
		using tagsm_t = A3D::meta::data_list_builder<char, long>::type;
		using dlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::data_list, dl_t, tagsm_t, tags_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, int*>);
		static_assert(std::is_same_v<dlm_t::next_type::value_type, float*>);
		static_assert(std::is_same_v<dlm_t::next_type::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 2 reversed")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using tags_t = A3D::meta::data_list_builder<char, long>::type;
		using tagsm_t = A3D::meta::data_list_builder<long, char>::type;
		using dlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::data_list, dl_t, tagsm_t, tags_t>::type;
		static_assert(std::is_same_v<dlm_t::value_type, float*>);
		static_assert(std::is_same_v<dlm_t::next_type::value_type, int*>);
		static_assert(std::is_same_v<dlm_t::next_type::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Get from size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		REQUIRE(A3D::meta::get<int>(dl) == 50);
	}

	TEST_CASE("Get from size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		A3D::meta::get<float>(dl) = 2.5f;
		REQUIRE(A3D::meta::get<int>(dl) == 50);
		REQUIRE(A3D::meta::get<float>(dl) == 2.5f);
	}

	TEST_CASE("Modify and get from size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		dlm_t dlm;
		A3D::meta::get<int*>(dlm) = &A3D::meta::get<int>(dl);
		REQUIRE(*A3D::meta::get<int*>(dlm) == 50);
	}

	TEST_CASE("Modify and get from size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		A3D::meta::get<float>(dl) = 2.5f;
		dlm_t dlm;
		A3D::meta::get<int*>(dlm) = &A3D::meta::get<int>(dl);
		A3D::meta::get<float*>(dlm) = &A3D::meta::get<float>(dl);
		REQUIRE(*A3D::meta::get<int*>(dlm) == 50);
		REQUIRE(*A3D::meta::get<float*>(dlm) == 2.5f);
	}

	TEST_CASE("Get by tag from size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using tags_t = A3D::meta::types_list_builder<char>::type;
		dl_t dl;
		A3D::meta::get_tag<char, tags_t, dl_t>(dl) = 50;
		REQUIRE(A3D::meta::get_tag<char, tags_t, dl_t>(dl) == 50);
		REQUIRE(A3D::meta::get<int>(dl) == 50);
	}

	TEST_CASE("Get by tag from size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using tags_t = A3D::meta::types_list_builder<char, long>::type;
		dl_t dl;
		A3D::meta::get_tag<char, tags_t, dl_t>(dl) = 50;
		A3D::meta::get_tag<long, tags_t, dl_t>(dl) = 2.5f;
		REQUIRE(A3D::meta::get_tag<char, tags_t, dl_t>(dl) == 50);
		REQUIRE(A3D::meta::get_tag<long, tags_t, dl_t>(dl) == 2.5f);
		REQUIRE(A3D::meta::get<int>(dl) == 50);
		REQUIRE(A3D::meta::get<float>(dl) == 2.5f);
	}

	TEST_CASE("Modify and get by tag from size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		using tags_t = A3D::meta::types_list_builder<char>::type;
		using tagsm_t = A3D::meta::types_list_builder<char>::type;
		dl_t dl;
		A3D::meta::get_tag<char, tags_t, dl_t>(dl) = 50;
		dlm_t dlm;
		A3D::meta::get_tag<char, tagsm_t, dlm_t>(dlm) = &A3D::meta::get_tag<char, tags_t, dl_t>(dl);
		REQUIRE(*A3D::meta::get_tag<char, tagsm_t, dlm_t>(dlm) == 50);
		REQUIRE(A3D::meta::get<int>(dl) == 50);
	}

	TEST_CASE("Modify and get by tag from size 2 ordered")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		using dlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::data_list, dl_t>::type;
		using tags_t = A3D::meta::types_list_builder<char, long>::type;
		using tagsm_t = A3D::meta::types_list_builder<char, long>::type;
		dl_t dl;
		A3D::meta::get_tag<char, tags_t, dl_t>(dl) = 50;
		A3D::meta::get_tag<long, tags_t, dl_t>(dl) = 2.5f;
		dlm_t dlm;
		A3D::meta::get_tag<char, tagsm_t, dlm_t>(dlm) = &A3D::meta::get_tag<char, tags_t, dl_t>(dl);
		A3D::meta::get_tag<long, tagsm_t, dlm_t>(dlm) = &A3D::meta::get_tag<long, tags_t, dl_t>(dl);
		REQUIRE(*A3D::meta::get_tag<char, tagsm_t, dlm_t>(dlm) == 50);
		REQUIRE(*A3D::meta::get_tag<long, tagsm_t, dlm_t>(dlm) == 2.5f);
		REQUIRE(A3D::meta::get<int>(dl) == 50);
		REQUIRE(A3D::meta::get<float>(dl) == 2.5f);
	}

	TEST_CASE("Foreach unary empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		dl_t dl;
		A3D::meta::foreach(dl, test_unary{});
	}

	TEST_CASE("Foreach unary size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		A3D::meta::foreach(dl, test_unary{});
		REQUIRE(A3D::meta::get<int>(dl) == 51);
	}

	TEST_CASE("Foreach unary size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		dl_t dl;
		A3D::meta::get<int>(dl) = 50;
		A3D::meta::get<float>(dl) = 2.5f;
		A3D::meta::foreach(dl, test_unary{});
		REQUIRE(A3D::meta::get<int>(dl) == 51);
		REQUIRE(A3D::meta::get<float>(dl) == 3.5f);
	}

	TEST_CASE("Foreach binary empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::foreach(ldl, rdl, test_binary{});
	}

	TEST_CASE("Foreach binary size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach binary size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(ldl) == 6.0f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}

	TEST_CASE("Foreach ternary empty")
	{
		using dl_t = A3D::meta::data_list_builder<>::type;
		dl_t res;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::foreach(res, ldl, rdl, test_ternary{});
	}

	TEST_CASE("Foreach ternary size 1")
	{
		using dl_t = A3D::meta::data_list_builder<int>::type;
		dl_t res;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach ternary size 2")
	{
		using dl_t = A3D::meta::data_list_builder<int, float>::type;
		dl_t res;
		dl_t ldl;
		dl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(res) == 6.0f);
		REQUIRE(A3D::meta::get<float>(ldl) == 2.5f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}

	TEST_CASE("Foreach binary type empty")
	{
		using ldl_t = A3D::meta::data_list_builder<>::type;
		using rdl_t = A3D::meta::data_list_builder<>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::foreach_type(ldl, rdl, test_binary{});
	}

	TEST_CASE("Foreach binary type size 1")
	{
		using ldl_t = A3D::meta::data_list_builder<int>::type;
		using rdl_t = A3D::meta::data_list_builder<int>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach_type(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach binary type size 2")
	{
		using ldl_t = A3D::meta::data_list_builder<int, float>::type;
		using rdl_t = A3D::meta::data_list_builder<float, int>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach_type(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(ldl) == 6.0f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}

	TEST_CASE("Foreach ternary type empty")
	{
		using res_t = A3D::meta::data_list_builder<>::type;
		using ldl_t = A3D::meta::data_list_builder<>::type;
		using rdl_t = A3D::meta::data_list_builder<>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::foreach_type(res, ldl, rdl, test_ternary{});
	}

	TEST_CASE("Foreach ternary type size 1")
	{
		using res_t = A3D::meta::data_list_builder<int>::type;
		using ldl_t = A3D::meta::data_list_builder<int>::type;
		using rdl_t = A3D::meta::data_list_builder<int>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach_type(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach ternary type size 2")
	{
		using res_t = A3D::meta::data_list_builder<int, float>::type;
		using ldl_t = A3D::meta::data_list_builder<float, int>::type;
		using rdl_t = A3D::meta::data_list_builder<float, int>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach_type(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(res) == 6.0f);
		REQUIRE(A3D::meta::get<float>(ldl) == 2.5f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}

	TEST_CASE("Foreach binary tags empty")
	{
		using ldl_t = A3D::meta::data_list_builder<>::type;
		using rdl_t = A3D::meta::data_list_builder<>::type;
		using ltags_t = A3D::meta::types_list_builder<>::type;
		using rtags_t = A3D::meta::types_list_builder<>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::foreach_tags<ltags_t, rtags_t>(ldl, rdl, test_binary{});
	}

	TEST_CASE("Foreach binary tags size 1")
	{
		using ldl_t = A3D::meta::data_list_builder<int>::type;
		using rdl_t = A3D::meta::data_list_builder<int>::type;
		using ltags_t = A3D::meta::types_list_builder<char>::type;
		using rtags_t = A3D::meta::types_list_builder<char>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach_tags<ltags_t, rtags_t>(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach binary tags size 2")
	{
		using ldl_t = A3D::meta::data_list_builder<int, float>::type;
		using rdl_t = A3D::meta::data_list_builder<float, int>::type;
		using ltags_t = A3D::meta::types_list_builder<char, long>::type;
		using rtags_t = A3D::meta::types_list_builder<long, char>::type;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach_tags<ltags_t, rtags_t>(ldl, rdl, test_binary{});
		REQUIRE(A3D::meta::get<int>(ldl) == 55);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(ldl) == 6.0f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}

	TEST_CASE("Foreach ternary tags empty")
	{
		using res_t = A3D::meta::data_list_builder<>::type;
		using ldl_t = A3D::meta::data_list_builder<>::type;
		using rdl_t = A3D::meta::data_list_builder<>::type;
		using restags_t = A3D::meta::types_list_builder<>::type;
		using ltags_t = A3D::meta::types_list_builder<>::type;
		using rtags_t = A3D::meta::types_list_builder<>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::foreach_tags<restags_t, ltags_t, rtags_t>(res, ldl, rdl, test_ternary{});
	}

	TEST_CASE("Foreach ternary tags size 1")
	{
		using res_t = A3D::meta::data_list_builder<int>::type;
		using ldl_t = A3D::meta::data_list_builder<int>::type;
		using rdl_t = A3D::meta::data_list_builder<int>::type;
		using restags_t = A3D::meta::types_list_builder<char>::type;
		using ltags_t = A3D::meta::types_list_builder<char>::type;
		using rtags_t = A3D::meta::types_list_builder<char>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::foreach_tags<restags_t, ltags_t, rtags_t>(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
	}

	TEST_CASE("Foreach ternary tags size 2")
	{
		using res_t = A3D::meta::data_list_builder<int, float>::type;
		using ldl_t = A3D::meta::data_list_builder<float, int>::type;
		using rdl_t = A3D::meta::data_list_builder<float, int>::type;
		using restags_t = A3D::meta::types_list_builder<char, long>::type;
		using ltags_t = A3D::meta::types_list_builder<long, char>::type;
		using rtags_t = A3D::meta::types_list_builder<long, char>::type;
		res_t res;
		ldl_t ldl;
		rdl_t rdl;
		A3D::meta::get<int>(ldl) = 50;
		A3D::meta::get<int>(rdl) = 5;
		A3D::meta::get<float>(ldl) = 2.5f;
		A3D::meta::get<float>(rdl) = 3.5f;
		A3D::meta::foreach_tags<restags_t, ltags_t, rtags_t>(res, ldl, rdl, test_ternary{});
		REQUIRE(A3D::meta::get<int>(res) == 55);
		REQUIRE(A3D::meta::get<int>(ldl) == 50);
		REQUIRE(A3D::meta::get<int>(rdl) == 5);
		REQUIRE(A3D::meta::get<float>(res) == 6.0f);
		REQUIRE(A3D::meta::get<float>(ldl) == 2.5f);
		REQUIRE(A3D::meta::get<float>(rdl) == 3.5f);
	}
}
