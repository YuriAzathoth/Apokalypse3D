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

TEST_SUITE("Types List")
{
	TEST_CASE("Empty List")
	{
		using tl_t = A3D::meta::types_list_builder<>::type;
		static_assert(std::is_same_v<tl_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<tl_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("List with size 1")
	{
		using tl_t = A3D::meta::types_list_builder<int>::type;
		static_assert(std::is_same_v<tl_t::value_type, int>);
		static_assert(std::is_same_v<tl_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("List with size 2")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		static_assert(std::is_same_v<tl_t::value_type, int>);
		static_assert(std::is_same_v<tl_t::next_type::value_type, float>);
		static_assert(std::is_same_v<tl_t::next_type::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Modify empty")
	{
		using tl_t = A3D::meta::types_list_builder<>::type;
		using tlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::types_list, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<tlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Modify size 1")
	{
		using tl_t = A3D::meta::types_list_builder<int>::type;
		using tlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::types_list, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Modify size 2")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		using tlm_t = A3D::meta::list_modify<std::add_pointer, A3D::meta::types_list, tl_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::next_type::value_type, float*>);
		static_assert(std::is_same_v<tlm_t::next_type::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Sizeof empty")
	{
		using tl_t = A3D::meta::types_list_builder<>::type;
		static_assert(A3D::meta::list_sizeof<tl_t>() == 0);
	}

	TEST_CASE("Sizeof size 1")
	{
		using tl_t = A3D::meta::types_list_builder<int>::type;
		static_assert(A3D::meta::list_sizeof<tl_t>() == sizeof(int));
	}

	TEST_CASE("Sizeof size 2")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		static_assert(A3D::meta::list_sizeof<tl_t>() == sizeof(int) + sizeof(float));
	}

	TEST_CASE("Type by tag size 1")
	{
		using tl_t = A3D::meta::types_list_builder<int>::type;
		using tags_t = A3D::meta::types_list_builder<char>::type;
		static_assert(std::is_same_v<A3D::meta::type_by_tag<char, tags_t, tl_t>::type, int>);
	}

	TEST_CASE("Type by tag size 2")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		using tags_t = A3D::meta::types_list_builder<char, long>::type;
		static_assert(std::is_same_v<A3D::meta::type_by_tag<char, tags_t, tl_t>::type, int>);
		static_assert(std::is_same_v<A3D::meta::type_by_tag<long, tags_t, tl_t>::type, float>);
	}

	TEST_CASE("Cast by tags empty")
	{
		using tl_t = A3D::meta::types_list_builder<>::type;
		using tags_t = A3D::meta::types_list_builder<>::type;
		using tlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::types_list, tl_t, tags_t, tags_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, A3D::meta::empty_type>);
		static_assert(std::is_same_v<tlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 1")
	{
		using tl_t = A3D::meta::types_list_builder<int>::type;
		using tags_t = A3D::meta::types_list_builder<char>::type;
		using tlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::types_list, tl_t, tags_t, tags_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 2 ordered")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		using tags_t = A3D::meta::types_list_builder<char, long>::type;
		using tagsm_t = A3D::meta::types_list_builder<char, long>::type;
		using tlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::types_list, tl_t, tagsm_t, tags_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::next_type::value_type, float*>);
		static_assert(std::is_same_v<tlm_t::next_type::next_type, A3D::meta::empty_type>);
	}

	TEST_CASE("Type by tag size 2 reversed")
	{
		using tl_t = A3D::meta::types_list_builder<int, float>::type;
		using tags_t = A3D::meta::types_list_builder<char, long>::type;
		using tagsm_t = A3D::meta::types_list_builder<long, char>::type;
		using tlm_t = A3D::meta::tags_modify<std::add_pointer, A3D::meta::types_list, tl_t, tagsm_t, tags_t>::type;
		static_assert(std::is_same_v<tlm_t::value_type, float*>);
		static_assert(std::is_same_v<tlm_t::next_type::value_type, int*>);
		static_assert(std::is_same_v<tlm_t::next_type::next_type, A3D::meta::empty_type>);
	}
}
