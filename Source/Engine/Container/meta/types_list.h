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

#ifndef CONTAINER_META_TYPES_LIST_H
#define CONTAINER_META_TYPES_LIST_H

#include <type_traits>

namespace A3D
{
namespace meta
{
// =========================================
// Forward declarations
// =========================================

template <typename...> struct data_list;
template <typename...> struct types_list;

// =========================================
// Compile time constants
// =========================================

struct true_type {};
struct false_type {};
struct empty_type {};

// =========================================
// Types list
// =========================================

template <typename T, typename... TNext>
struct types_list<T, TNext...>
{
	using value_type = T;
	using next_type = types_list<TNext...>;
	using is_last = false_type;
	using is_empty = false_type;
	using to_data_list = data_list<T, TNext...>;
	template <typename... TArgs> using add_types = types_list<TArgs..., T, TNext...>;
};

template <typename T>
struct types_list<T>
{
	using value_type = T;
	using is_last = true_type;
	using is_empty = false_type;
	using to_data_list = data_list<T>;
	template <typename... TArgs> using add_types = types_list<TArgs..., T>;
};

template <>
struct types_list<>
{
	using value_type = void;
	using is_last = true_type;
	using is_empty = true_type;
	using to_data_list = data_list<>;
	template <typename... TArgs> using add_types = types_list<TArgs...>;
};

// =========================================
// Cast types in types list to another
// =========================================

template <typename T>
struct modifier_bypass
{
	using type = T;
};

template <template <typename> typename Modifier, typename TypesList>
struct types_list_modify
{
	using adding_type = typename TypesList::value_type;
	using adding_type_modified = typename Modifier<adding_type>::type;
	using next_types_list = typename TypesList::next_type;
	using next_types_list_modified = typename types_list_modify<Modifier, next_types_list>::type;
	using type = typename next_types_list_modified::add_types<adding_type_modified>;
};

template <template <typename> typename Modifier, typename T>
struct types_list_modify<Modifier, types_list<T>>
{
	using adding_type = typename types_list<T>::value_type;
	using adding_type_modified = typename Modifier<adding_type>::type;
	using type = types_list<adding_type_modified>;
};

template <template <typename> typename Modifier>
struct types_list_modify<Modifier, types_list<>>
{
	using type = types_list<>;
};

// =========================================
// Compute size of all elements in types list
// =========================================

template <typename TypesList>
struct types_list_sizeof
{
	using value_type = typename TypesList::value_type;
	using next_type = typename TypesList::next_type;
	enum { value = sizeof(value_type) + types_list_sizeof<next_type>::value };
};

template <typename T>
struct types_list_sizeof<types_list<T>>
{
	enum { value = sizeof(T) };
};

template <>
struct types_list_sizeof<types_list<>>
{
	enum { value = 0 };
};

// =========================================
// Get types list type by tag
// =========================================

template <typename Tag, typename TagsList, typename TypesList>
struct types_list_type_by_tag
{
	using next_type = typename TypesList::next_type;
	using next_tags = typename TagsList::next_type;
	using type = types_list_type_by_tag<Tag, next_tags, next_type>::type;
};

template <typename Tag, typename TypesList, typename... TArgs>
struct types_list_type_by_tag<Tag, types_list<Tag, TArgs...>, TypesList>
{
	using type = typename TypesList::value_type;
};

template <typename Tag, typename TypesList>
struct types_list_type_by_tag<Tag, TypesList, types_list<>>
{
	using type = void;
};

template <typename Tag, typename T, typename... TTypes>
struct types_list_type_by_tag<Tag, types_list<>, types_list<T, TTypes...>>
{
	using type = void;
};

// =========================================
// Cast types in types list by tags
// =========================================

template <template <typename> typename Modifier,
		  typename SelectedTags,
		  typename AllTags,
		  typename TypesList>
struct types_list_modify_by_tags
{
	using current_tag = typename SelectedTags::value_type;
	using adding_type = typename types_list_type_by_tag<current_tag, AllTags, TypesList>::type;
	using adding_type_modified = typename Modifier<adding_type>::type;
	using next_tags = typename SelectedTags::next_type;
	using next_types_list_modified = typename types_list_modify_by_tags<Modifier, next_tags, AllTags, TypesList>::type;
	using type = typename next_types_list_modified::add_types<adding_type_modified>;
};

// Variant if selected tags contains last element.
template <template <typename> typename Modifier,
		  typename T,
		  typename AllTags,
		  typename TypesList>
struct types_list_modify_by_tags<Modifier, types_list<T>, AllTags, TypesList>
{
	using current_tag = T;
	using adding_type = typename types_list_type_by_tag<current_tag, AllTags, TypesList>::type;
	using adding_type_modified = typename Modifier<adding_type>::type;
	using type = types_list<adding_type_modified>;
};

// Variant if types list and tags are empty.
template <template <typename> typename Modifier, typename T, typename... TTags>
struct types_list_modify_by_tags<Modifier, types_list<T, TTags...>, types_list<>, types_list<>>
{
	using type = types_list<>;
};

// Variant if selected tags is empty.
template <template <typename> typename Modifier, typename AllTags, typename TypesList>
struct types_list_modify_by_tags<Modifier, types_list<>, AllTags, TypesList>
{
	using type = types_list<>;
};

// =========================================
// Create new types list from old by tags
// =========================================

template <typename SelectedTags, typename AllTags, typename TypesList>
struct types_list_select_by_tags
{
	using type = typename types_list_modify_by_tags<modifier_bypass, SelectedTags, AllTags, TypesList>::type;
};

} // namespace meta
} // namespace A3D

#endif // CONTAINER_META_TYPES_LIST_H
