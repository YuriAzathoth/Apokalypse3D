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

#ifndef CONTAINER_META_DATABASE_TABLE_H
#define CONTAINER_META_DATABASE_TABLE_H

#include <stdint.h>
#include "Container/cpp_lifecycle.h"
#include "types_list.h"

namespace A3D
{
namespace db
{
namespace table
{
template <typename TypesList>
using table = meta::list_modify<std::add_pointer, meta::data_list, TypesList>::type;

template <typename Table>
inline constexpr size_t memory_size(size_t column_count)
{
	return column_count * meta::list_sizeof<typename meta::list_modify<std::remove_pointer, meta::data_list, Table>::type>();
}

class distribute_memory
{
public:
	distribute_memory(uint8_t* mem, size_t rows_count) : mem_(mem), rows_count_(rows_count) {}

	template <typename T>
	void operator()(T*& ptr)
	{
		ptr = reinterpret_cast<T*>(mem_);
		mem_ += rows_count_ * sizeof(T);
	}

private:
	uint8_t* mem_;
	size_t rows_count_;
};

struct construct_value
{
	template <typename T>
	void operator()(T* dst)
	{
		A3D::construct(dst);
	}
};

struct destroy_value
{
	template <typename T>
	void operator()(T* dst)
	{
		A3D::destroy(dst);
	}
};

class construct_data
{
public:
	construct_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* dst)
	{
		A3D::construct_n(dst, size_);
	}

private:
	size_t size_;
};

class destroy_data
{
public:
	destroy_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* dst)
	{
		A3D::destroy_n(dst, size_);
	}

private:
	size_t size_;
};

class copy_data
{
public:
	copy_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* dst, T* src)
	{
		A3D::copy_construct_n(dst, src, size_);
	}

private:
	size_t size_;
};

class move_data
{
public:
	move_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* dst, T* src)
	{
		A3D::move_construct_n(dst, src, size_);
		A3D::destroy_n(src, size_);
	}

private:
	size_t size_;
};

struct set_pointer
{
	template <typename T>
	void operator()(T*& dst, T* src)
	{
		dst = src;
	}
};

class set_pointer_bias
{
public:
	set_pointer_bias(ptrdiff_t bias) : bias_(bias) {}

	template <typename T>
	void operator()(T*& dst, T* src)
	{
		dst = src + bias_;
	}

private:
	ptrdiff_t bias_;
};

struct reset_pointer
{
	template <typename T>
	void operator()(T*& dst)
	{
		dst = nullptr;
	}
};

struct increment_pointer
{
	template <typename T>
	void operator()(T*& dst)
	{
		++dst;
	}
};

struct decrement_pointer
{
	template <typename T>
	void operator()(T*& dst)
	{
		++dst;
	}
};

template <template <typename> typename Modifier,
		  typename Table,
		  typename TableTagsList,
		  typename IteratorTagsList>
class iterator_base
{
public:
	using data_types_table = Table;
	using tags_types_table = TableTagsList;

	using tags_types_iter = IteratorTagsList;
	using data_types_iter = meta::tags_modify
	<
		Modifier,
		meta::data_list,
		data_types_table,
		tags_types_iter,
		tags_types_table
	>::type;

	using pointer_iter = meta::list_modify
	<
		std::add_pointer,
		meta::data_list,
		data_types_iter
	>::type;

	iterator_base() noexcept
	{
		meta::foreach(ptr_, reset_pointer{});
	}

	template <typename DataList>
	iterator_base(DataList& ptr) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer{});
	}

	iterator_base(pointer_iter& ptr) noexcept
	{
		meta::foreach(ptr_, ptr, set_pointer{});
	}

	template <typename DataList>
	iterator_base(DataList& ptr, ptrdiff_t bias) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer_bias{bias});
	}

	iterator_base(pointer_iter& ptr, ptrdiff_t bias) noexcept
	{
		meta::foreach(ptr_, ptr, set_pointer_bias{bias});
	}

	template <typename DataList>
	void operator=(DataList& ptr) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer{});
	}

	void operator=(pointer_iter& ptr) noexcept
	{
		meta::foreach(ptr_, ptr, set_pointer{});
	}

	void operator++() noexcept
	{
		meta::foreach(ptr_, increment_pointer{});
	}

	void operator--() noexcept
	{
		meta::foreach(ptr_, decrement_pointer{});
	}

	void operator+=(ptrdiff_t delta) noexcept
	{
		meta::foreach(ptr_, set_pointer_bias{delta});
	}

	void operator-=(ptrdiff_t delta) noexcept
	{
		meta::foreach(ptr_, set_pointer_bias{-delta});
	}

	iterator_base operator+(ptrdiff_t delta) noexcept
	{
		iterator_base ret;
		meta::foreach(ret, ptr_, set_pointer_bias{delta});
		return ret;
	}

	iterator_base operator-(ptrdiff_t delta) noexcept
	{
		iterator_base ret;
		meta::foreach(ret, ptr_, set_pointer_bias{-delta});
		return ret;
	}

	bool operator==(const iterator_base& other) const noexcept
	{
		return ptr_.value == other.ptr_.value;
	}

	bool operator!=(const iterator_base& other) const noexcept
	{
		return ptr_.value != other.ptr_.value;
	}

	template <typename DataList>
	bool operator==(const DataList& other) const noexcept
	{
		return ptr_.value == meta::get_tag<typename tags_types_iter::value_type, tags_types_table>(other);
	}

	template <typename DataList>
	bool operator!=(const DataList& other) const noexcept
	{
		return ptr_.value != meta::get_tag<typename tags_types_iter::value_type, tags_types_table>(other);
	}

	template <typename Tag>
	typename meta::type_by_tag<Tag, tags_types_iter, data_types_iter>::type&
	get() noexcept
	{
		return *meta::get_tag<Tag, tags_types_iter>(ptr_);
	}

	pointer_iter& ptr() noexcept { return ptr_; }
	const pointer_iter& ptr() const noexcept { return ptr_; }

private:
	pointer_iter ptr_;
};

template <typename Table, typename TableTags, typename Tags>
using iterator = iterator_base<meta::bypass, Table, TableTags, Tags>;

template <typename Table, typename TableTags, typename Tags>
using const_iterator = iterator_base<std::add_const, Table, TableTags, Tags>;
} // namespace table
} // namespace db
} // namespace A3D

#endif // CONTAINER_META_DATABASE_TABLE_H
