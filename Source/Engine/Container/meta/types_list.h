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
struct true_type {};
struct false_type {};
struct empty_type {};


template <typename T, typename Next>
struct types_list
{
	using value_type = T;
	using next_type = Next;
	using is_last = false_type;
};

template <typename T>
struct types_list<T, empty_type>
{
	using value_type = T;
	using next_type = empty_type;
	using is_last = true_type;
};

template <>
struct types_list<empty_type, empty_type>
{
	using value_type = empty_type;
	using next_type = empty_type;
	using is_last = true_type;
};


template <typename T, typename Next>
struct data_list
{
	using value_type = T;
	using next_type = Next;
	using is_last = false_type;

	value_type value;
	next_type next;
};

template <typename T>
struct data_list<T, empty_type>
{
	using value_type = T;
	using next_type = empty_type;
	using is_last = true_type;

	value_type value;
};

template <>
struct data_list<empty_type, empty_type>
{
	using value_type = empty_type;
	using next_type = empty_type;
	using is_last = true_type;
};


template <template <typename, typename> typename List, typename... Types> struct list_builder;

template <template <typename, typename> typename List, typename T, typename... Types>
struct list_builder<List, T, Types...>
{
	using current = T;
	using next = typename list_builder<List, Types...>::type;
	using type = List<current, next>;

	template <typename U> using add = list_builder<List, T, Types..., U>;
};

template <template <typename, typename> typename List, typename T>
struct list_builder<List, T>
{
	using current = T;
	using next = empty_type;
	using type = List<current, next>;

	template <typename U> using add = list_builder<List, T, U>;
};

template <template <typename, typename> typename List>
struct list_builder<List>
{
	using current = empty_type;
	using next = empty_type;
	using type = List<current, next>;

	template <typename U> using add = list_builder<List, U>;
};

template <typename... Types>
using types_list_builder = list_builder<types_list, Types...>;

template <typename... Types>
using data_list_builder = list_builder<data_list, Types...>;

template <typename... Types>
using tuple = typename list_builder<data_list, Types...>::type;


template <template <typename, typename> typename DstList, typename List1, typename List2>
struct list_join
{
	using type = DstList<typename List1::value_type, typename list_join<DstList, typename List1::next_type, List2>::type>;
};

template <template <typename, typename> typename DstList, typename T, template <typename, typename> typename List1, typename List2>
struct list_join<DstList, List1<T, empty_type>, List2>
{
	using type = DstList<T, List2>;
};


template <typename T>
struct bypass
{
	using type = T;
};


template <template <typename> typename Modifier,
		  template <typename, typename> typename DstList,
		  typename SrcList>
struct list_modify
{
	using current = typename Modifier<typename SrcList::value_type>::type;
	using next = typename list_modify<Modifier, DstList, typename SrcList::next_type>::type;
	using type = DstList<current, next>;
};

template <template <typename> typename Modifier,
		  template <typename, typename> typename DstList,
		  typename T,
		  template <typename, typename> typename SrcList>
struct list_modify<Modifier, DstList, SrcList<T, empty_type>>
{
	using current = typename Modifier<typename SrcList<T, empty_type>::value_type>::type;
	using next = empty_type;
	using type = DstList<current, next>;
};

template <template <typename> typename Modifier,
		  template <typename, typename> typename DstList,
		  template <typename, typename> typename SrcList>
struct list_modify<Modifier, DstList, SrcList<empty_type, empty_type>>
{
	using current = empty_type;
	using next = empty_type;
	using type = types_list<current, next>;
};


template <template <typename, typename> typename DstList, typename SrcList>
using list_cast = list_modify<bypass, DstList, SrcList>;


template <typename SrcList> using make_data_list = typename list_cast<data_list, SrcList>::type;
template <typename SrcList> using make_types_list = typename list_cast<types_list, SrcList>::type;


template <typename List>
constexpr size_t list_sizeof()
{
	using current = typename List::value_type;
	using next = typename List::next_type;
	if constexpr(std::is_same<typename List::is_last, false_type>::value)
		return sizeof(current) + list_sizeof<next>();
	else if constexpr(std::is_same<typename List::value_type, empty_type>::value)
		return 0;
	else
		return sizeof(current);
};


template <typename T, typename List>
constexpr T& get(List& list)
{
	if constexpr (std::is_same<T, typename List::value_type>::value)
		return list.value;
	else
		return get<T>(list.next);
}


template <typename List, typename UnaryOp>
constexpr void foreach(List& list, UnaryOp&& op)
{
	if constexpr (!std::is_same<typename List::value_type, empty_type>::value)
		op(list.value);
	if constexpr (std::is_same<typename List::is_last, false_type>::value)
		foreach(list.next, op);
}

template <typename List1, typename List2, typename BinaryOp>
constexpr void foreach(List1& list1, List2& list2, BinaryOp&& op)
{
	if constexpr (!std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value)
		op(list1.value, list2.value);
	if constexpr (std::is_same<typename List1::is_last, false_type>::value)
		foreach(list1.next, list2.next, op);
}

template <typename List1, typename List2, typename List3, typename TernaryOp>
constexpr void foreach(List1& list1, List2& list2, List3& list3 , TernaryOp&& op)
{
	if constexpr (!std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value &&
				   !std::is_same<typename List3::value_type, empty_type>::value)
		op(list1.value, list2.value, list3.value);
	if constexpr (std::is_same<typename List1::is_last, false_type>::value)
		foreach(list1.next, list2.next, list3.next, op);
}


template <typename List1, typename List2, typename BinaryOp>
constexpr void foreach_type(List1& list1, List2& list2, BinaryOp&& op)
{
	if constexpr (!std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value)
		op(list1.value, get<typename List1::value_type>(list2));
	if constexpr (std::is_same<typename List1::is_last, false_type>::value)
		foreach(list1.next, list2, op);
}

template <typename List1, typename List2, typename List3, typename TernaryOp>
constexpr void foreach_type(List1& list1, List2& list2, List3& list3 , TernaryOp&& op)
{
	if constexpr (!std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value &&
				   !std::is_same<typename List3::value_type, empty_type>::value)
		op(list1.value, get<typename List1::value_type>(list2), get<typename List1::value_type>(list3));
	if constexpr (std::is_same<typename List1::is_last, false_type>::value)
		foreach(list1.next, list2, list3, op);
}




template <typename Tag, typename TagsList, typename List>
struct type_by_tag
{
	using next_list = typename List::next_type;
	using next_tags = typename TagsList::next_type;
	using type = typename type_by_tag<Tag, next_tags, next_list>::type;
};

template <typename Tag, template <typename, typename> typename TagsList, typename List, typename TagsListRest>
struct type_by_tag<Tag, TagsList<Tag, TagsListRest>, List>
{
	using type = typename List::value_type;
};


template <typename Tag, typename TagsList, typename List>
constexpr
typename type_by_tag<Tag, TagsList, List>::type&
get_tag(List& list)
{
	if constexpr (std::is_same<Tag, typename TagsList::value_type>::value)
		return list.value;
	else
		return get_tag<Tag, typename TagsList::next_type>(list.next);
}


template <
	template <typename> typename Modifier,
	template <typename, typename> typename DstList,
	typename SrcList,
	typename DstTagsList,
	typename SrcTagsList>
struct tags_modify
{
	using current = typename Modifier<typename type_by_tag<typename DstTagsList::value_type, SrcTagsList, SrcList>::type>::type;
	using next = typename tags_modify<Modifier, DstList, SrcList, typename DstTagsList::next_type, SrcTagsList>::type;
	using type = DstList<current, next>;
};

template <
	template <typename> typename Modifier,
	template <typename, typename> typename DstList,
	typename SrcList,
	template <typename, typename> typename DstTagsList,
	typename SrcTagsList,
	typename T>
struct tags_modify<Modifier, DstList, SrcList, DstTagsList<T, empty_type>, SrcTagsList>
{
	using current = typename Modifier<typename type_by_tag<T, SrcTagsList, SrcList>::type>::type;
	using next = empty_type;
	using type = DstList<current, next>;
};

template <
	template <typename> typename Modifier,
	template <typename, typename> typename DstList,
	typename SrcList,
	template <typename, typename> typename DstTagsList,
	typename SrcTagsList>
struct tags_modify<Modifier, DstList, SrcList, DstTagsList<empty_type, empty_type>, SrcTagsList>
{
	using current = empty_type;
	using next = empty_type;
	using type = DstList<current, next>;
};


template <template <typename, typename> typename DstList, typename SrcList, typename DstTagsList, typename SrcTagsList>
using tags_cast = tags_modify<bypass, DstList, SrcList, DstTagsList, SrcTagsList>;


template <
	typename TagsList1,
	typename TagsList2,
	typename List1,
	typename List2,
	typename BinaryOp>
constexpr void foreach_tags(List1& lhs, List2& rhs, BinaryOp&& op)
{
	if constexpr (!std::is_same<typename TagsList1::value_type, empty_type>::value &&
				   !std::is_same<typename TagsList2::value_type, empty_type>::value &&
				   !std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value)
		op(lhs.value, get_tag<typename TagsList1::value_type, TagsList2>(rhs));
	if constexpr (std::is_same<typename List1::is_last, false_type>::value &&
				   std::is_same<typename TagsList1::is_last, false_type>::value)
		foreach_tags<typename TagsList1::next_type, TagsList2>(lhs.next, rhs, op);
}

template <
	typename TagsList1,
	typename TagsList2,
	typename TagsList3,
	typename List1,
	typename List2,
	typename List3,
	typename TernaryOp>
constexpr void foreach_tags(List1& res, List2& lhs, List3& rhs, TernaryOp&& op)
{
	if constexpr (!std::is_same<typename TagsList1::value_type, empty_type>::value &&
				   !std::is_same<typename TagsList2::value_type, empty_type>::value &&
				   !std::is_same<typename TagsList3::value_type, empty_type>::value &&
				   !std::is_same<typename List1::value_type, empty_type>::value &&
				   !std::is_same<typename List2::value_type, empty_type>::value &&
				   !std::is_same<typename List3::value_type, empty_type>::value)
		op(res.value,
			get_tag<typename TagsList1::value_type, TagsList2>(lhs),
			get_tag<typename TagsList1::value_type, TagsList3>(rhs));
	if constexpr (std::is_same<typename List1::is_last, false_type>::value &&
				   std::is_same<typename TagsList1::is_last, false_type>::value)
		foreach_tags<typename TagsList1::next_type, TagsList2, TagsList3>(res.next, lhs, rhs, op);
}
} // namespace meta
} // namespace A3D

#endif // CONTAINER_META_TYPES_LIST_H
