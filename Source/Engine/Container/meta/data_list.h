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

#ifndef CONTAINER_META_DATA_LIST_H
#define CONTAINER_META_DATA_LIST_H

#include <type_traits>
#include "types_list.h"

namespace A3D
{
namespace meta
{
// =========================================
// Data list
// =========================================

template <typename T, typename... TNext>
struct data_list<T, TNext...>
{
	using types = types_list<T, TNext...>;
	using value_type = typename types::value_type;
	using next_type = typename types::next_type::to_data_list;

	value_type value;
	next_type next;
};

template <typename T>
struct data_list<T>
{
	using types = types_list<T>;
	using value_type = typename types::value_type;

	value_type value;
};

template <>
struct data_list<>
{
	using types = types_list<>;
	using value_type = typename types::value_type;
};

// =========================================
// Get value by type from data list
// =========================================

template <typename T, typename DataList>
constexpr T& data_list_get(DataList& dl)
{
	if constexpr (std::is_same<typename DataList::value_type, T>::value)
		return dl.value;
	else
		return data_list_get<T>(dl.next);
}

template <typename T, typename DataList>
constexpr const T& data_list_get(const DataList& dl)
{
	if constexpr (std::is_same<typename DataList::value_type, T>::value)
		return dl.value;
	else
		return data_list_get<T>(dl.next);
}

// =========================================
// Iterate through data list items
// =========================================

template <typename DataList, typename Functor>
constexpr void data_list_foreach(DataList& dl, Functor&& unary_op)
{
	if constexpr (std::is_same<typename DataList::types::is_empty, false_type>::value)
		unary_op(dl.value);
	if constexpr (std::is_same<typename DataList::types::is_last, false_type>::value)
		data_list_foreach(dl.next, std::forward<Functor>(unary_op));
}

template <typename DataList1, typename DataList2, typename Functor>
constexpr void data_list_foreach(DataList1& dl1, DataList2& dl2, Functor&& binary_op)
{
	if constexpr (std::is_same<typename DataList1::types::is_empty, false_type>::value &&
		std::is_same<typename DataList2::types::is_empty, false_type>::value)
		binary_op(dl1.value, dl2.value);
	if constexpr (std::is_same<typename DataList1::types::is_last, false_type>::value &&
		std::is_same<typename DataList2::types::is_last, false_type>::value)
		data_list_foreach(dl1.next, dl2.next, std::forward<Functor>(binary_op));
}

template <typename DataList1, typename DataList2, typename DataList3, typename Functor>
constexpr void data_list_foreach(DataList1& dl1, DataList2& dl2, DataList3& dl3, Functor&& ternary_op)
{
	if constexpr (std::is_same<typename DataList1::types::is_empty, false_type>::value &&
		std::is_same<typename DataList2::types::is_empty, false_type>::value &&
		std::is_same<typename DataList2::types::is_empty, false_type>::value)
		ternary_op(dl1.value, dl2.value, dl3.value);
	if constexpr (std::is_same<typename DataList1::types::is_last, false_type>::value &&
		std::is_same<typename DataList2::types::is_last, false_type>::value &&
		std::is_same<typename DataList3::types::is_last, false_type>::value)
		data_list_foreach(dl1.next, dl2.next, dl3.next, std::forward<Functor>(ternary_op));
}

// =========================================
// Get value by tag from data list
// =========================================

template <typename Tag, typename TagsList, typename DataList>
constexpr
typename types_list_type_by_tag<Tag, TagsList, typename DataList::types>::type&
data_list_get_by_tag(DataList& dl)
{
	if constexpr (std::is_same<typename TagsList::value_type, Tag>::value)
		return dl.value;
	else
		return data_list_get_by_tag<Tag, typename TagsList::next_type>(dl.next);
}

template <typename Tag, typename TagsList, typename DataList>
constexpr const
typename types_list_type_by_tag<Tag, TagsList, typename DataList::types>::type&
data_list_get_by_tag(const DataList& dl)
{
	if constexpr (std::is_same<typename TagsList::value_type, Tag>::value)
		return dl.value;
	else
		return data_list_get_by_tag<Tag, typename TagsList::next_type>(dl.next);
}

// =========================================
// Iterate through data list items by tags
// =========================================

template <typename SelectedTags,
		  typename AllTags,
		  typename DataList,
		  typename Functor>
constexpr void data_list_foreach_by_tags(DataList& dl, Functor&& unary_op)
{
	if constexpr (std::is_same<typename DataList::types::is_empty, false_type>::value)
	{
		if constexpr (std::is_same<typename SelectedTags::is_empty, false_type>::value)
		{
			using current_tag = typename SelectedTags::value_type;
			unary_op(data_list_get_by_tag<current_tag, AllTags>(dl));
		}
		if constexpr (std::is_same<typename SelectedTags::is_last, false_type>::value)
		{
			using next_tags = typename SelectedTags::next_type;
			data_list_foreach_by_tags<next_tags, AllTags>(dl, std::forward<Functor>(unary_op));
		}
	}
}

template <typename SelectedTags,
		  typename AllTags,
		  typename DataList1,
		  typename DataList2,
		  typename Functor>
constexpr void data_list_foreach_by_tags(DataList1& dl1, DataList2& dl2, Functor&& binary_op)
{
	if constexpr (std::is_same<typename DataList1::types::is_empty, false_type>::value &&
				  std::is_same<typename DataList2::types::is_empty, false_type>::value)
	{
		if constexpr (std::is_same<typename SelectedTags::is_empty, false_type>::value)
		{
			using current_tag = typename SelectedTags::value_type;
			binary_op(data_list_get_by_tag<current_tag, AllTags>(dl1),
					  data_list_get_by_tag<current_tag, AllTags>(dl2));
		}
		if constexpr (std::is_same<typename SelectedTags::is_last, false_type>::value)
		{
			using next_tags = typename SelectedTags::next_type;
			data_list_foreach_by_tags<next_tags, AllTags>(dl1, dl2, std::forward<Functor>(binary_op));
		}
	}
}

template <typename SelectedTags,
		  typename AllTags,
		  typename DataList1,
		  typename DataList2,
		  typename DataList3,
		  typename Functor>
constexpr void data_list_foreach_by_tags(DataList1& dl1, DataList2& dl2, DataList3& dl3, Functor&& ternary_op)
{
	if constexpr (std::is_same<typename DataList1::types::is_empty, false_type>::value &&
				  std::is_same<typename DataList2::types::is_empty, false_type>::value &&
				  std::is_same<typename DataList3::types::is_empty, false_type>::value)
	{
		if constexpr (std::is_same<typename SelectedTags::is_empty, false_type>::value)
		{
			using current_tag = typename SelectedTags::value_type;
			ternary_op(data_list_get_by_tag<current_tag, AllTags>(dl1),
					   data_list_get_by_tag<current_tag, AllTags>(dl2),
					   data_list_get_by_tag<current_tag, AllTags>(dl3));
		}
		if constexpr (std::is_same<typename SelectedTags::is_last, false_type>::value)
		{
			using next_tags = typename SelectedTags::next_type;
			data_list_foreach_by_tags<next_tags, AllTags>(dl1, dl2, dl3, std::forward<Functor>(ternary_op));
		}
	}
}

} // namespace meta
} // namespace A3D

#endif // CONTAINER_META_DATA_LIST_H
