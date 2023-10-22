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
namespace data_table
{
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

	uint8_t* mem() const noexcept { return mem_; }

private:
	uint8_t* mem_;
	size_t rows_count_;
};

struct construct_value
{
	template <typename T>
	void operator()(T* ptr)
	{
		A3D::construct(ptr);
	}
};

struct destroy_value
{
	template <typename T>
	void operator()(T* ptr)
	{
		A3D::destroy(ptr);
	}
};

struct copy_value
{
	template <typename T>
	void operator()(T* dst, const T* src)
	{
		A3D::copy_assign(dst, src);
	}
};

struct move_value
{
	template <typename T>
	void operator()(T* dst, const T* src)
	{
		A3D::move_assign(dst, src);
	}
};

class construct_data
{
public:
	construct_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* ptr)
	{
		A3D::construct_n(ptr, size_);
	}

private:
	size_t size_;
};

class destroy_data
{
public:
	destroy_data(size_t size) : size_(size) {}

	template <typename T>
	void operator()(T* ptr)
	{
		A3D::destroy_n(ptr, size_);
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

	iterator_base() noexcept { meta::foreach(ptr_, reset_pointer{}); }
	iterator_base(const iterator_base& other) noexcept { meta::foreach(ptr_, other.ptr_, set_pointer{}); }
	iterator_base(pointer_iter& ptr) noexcept { meta::foreach(ptr_, ptr, set_pointer{}); }
	iterator_base(pointer_iter& ptr, ptrdiff_t bias) noexcept { meta::foreach(ptr_, ptr, set_pointer_bias{bias}); }

	template <typename DataList>
	iterator_base(DataList& ptr) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer{});
	}

	template <typename DataList>
	iterator_base(DataList& ptr, ptrdiff_t bias) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer_bias{bias});
	}

	void operator=(const iterator_base& other) noexcept
	{
		meta::foreach(ptr_, other.ptr_, set_pointer{});
	}

	template <typename DataList>
	void operator=(DataList& ptr) noexcept
	{
		meta::foreach_tags<tags_types_iter, tags_types_table>(ptr_, ptr, set_pointer{});
	}

	void operator=(pointer_iter& ptr) noexcept { meta::foreach(ptr_, ptr, set_pointer{}); }

	void operator++() noexcept { meta::foreach(ptr_, increment_pointer{}); }
	void operator--() noexcept { meta::foreach(ptr_, decrement_pointer{}); }
	void operator+=(ptrdiff_t delta) noexcept { meta::foreach(ptr_, set_pointer_bias{delta}); }
	void operator-=(ptrdiff_t delta) noexcept { meta::foreach(ptr_, set_pointer_bias{-delta}); }

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

	bool operator==(const iterator_base& other) const noexcept { return ptr_.value == other.ptr_.value; }
	bool operator!=(const iterator_base& other) const noexcept { return ptr_.value != other.ptr_.value; }

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

template <typename TypesList, typename SizeType>
class table
{
public:
	using data_type = TypesList;
	using const_data_type = meta::list_modify<std::add_const, meta::data_list, data_type>::type;
	using pointers_type = meta::list_modify<std::add_pointer, meta::data_list, data_type>::type;
	using const_pointers_type = meta::list_modify<std::add_pointer, meta::data_list, const_data_type>::type;
	using size_type = SizeType;

	template <typename Tags, typename TableTags> using iterator = iterator_base<meta::bypass, data_type, TableTags, Tags>;
	template <typename Tags, typename TableTags> using const_iterator = iterator_base<meta::bypass, data_type, TableTags, Tags>;

	template <typename Tags, typename TableTags> iterator<Tags, TableTags> begin() noexcept { return { data_ }; }
	template <typename Tags, typename TableTags> const_iterator<Tags, TableTags> begin() const noexcept { return { data_ }; }
	template <typename Tags, typename TableTags> const_iterator<Tags, TableTags> cbegin() const noexcept { return { data_ }; }
	template <typename Tags, typename TableTags> iterator<Tags, TableTags> mid(size_type id) noexcept { return { data_, id }; }
	template <typename Tags, typename TableTags> const_iterator<Tags, TableTags> mid(size_type id) const noexcept { return { data_, id }; }
	template <typename Tags, typename TableTags> const_iterator<Tags, TableTags> cmid(size_type id) const noexcept { return { data_, id }; }

	template <typename TableTags> iterator<TableTags, TableTags> abegin() noexcept { return { data_ }; }
	template <typename TableTags> const_iterator<TableTags, TableTags> abegin() const noexcept { return { data_ }; }
	template <typename TableTags> const_iterator<TableTags, TableTags> cabegin() const noexcept { return { data_ }; }
	template <typename TableTags> iterator<TableTags, TableTags> amid(size_type id) noexcept { return { data_, id }; }
	template <typename TableTags> const_iterator<TableTags, TableTags> amid(size_type id) const noexcept { return { data_, id }; }
	template <typename TableTags> const_iterator<TableTags, TableTags> camid(size_type id) const noexcept { return { data_, id }; }

	template <typename Tag, typename TableTags>
	typename meta::type_by_tag<Tag, TableTags, data_type>::type&
	front() noexcept { return *meta::get_tag<Tag, TableTags>(data_); }

	template <typename Tag, typename TableTags>
	const typename meta::type_by_tag<Tag, TableTags, const_data_type>::type&
	front() const noexcept { return *meta::get_tag<Tag, TableTags>(data_); }

	template <typename Tag, typename TableTags>
	const typename meta::type_by_tag<Tag, TableTags, const_data_type>::type&
	cfront() const noexcept { return *meta::get_tag<Tag, TableTags>(data_); }

	template <typename Tag, typename TableTags>
	typename meta::type_by_tag<Tag, TableTags, data_type>::type&
	at(size_type id) noexcept { return meta::get_tag<Tag, TableTags>(data_)[id]; }

	template <typename Tag, typename TableTags>
	typename meta::type_by_tag<Tag, TableTags, const_data_type>::type&
	at(size_type id) const noexcept { return meta::get_tag<Tag, TableTags>(data_)[id]; }

	template <typename Tag, typename TableTags>
	typename meta::type_by_tag<Tag, TableTags, const_data_type>::type&
	cat(size_type id) const noexcept { return meta::get_tag<Tag, TableTags>(data_)[id]; }

	pointers_type& data() noexcept { return data_; }
	const pointers_type& data() const noexcept { return data_; }

	uint8_t* allocate(uint8_t* mem, size_t rows_count) noexcept
	{
		distribute_memory dm(mem, rows_count);
		meta::foreach(data_, dm);
		return dm.mem();
	}

	template <typename TableTags>
	void create(iterator<TableTags, TableTags>& iter)
	{
		meta::foreach(iter, construct_value{});
	}

	template <typename TableTags>
	void destroy(iterator<TableTags, TableTags>& iter)
	{
		meta::foreach(iter, destroy_value{});
	}

	template <typename TableTags>
	void copy(iterator<TableTags, TableTags> dst, iterator<TableTags, TableTags> src)
	{
		meta::foreach(dst, src, copy_value{});
	}

	template <typename TableTags>
	void move(iterator<TableTags, TableTags> dst, iterator<TableTags, TableTags> src)
	{
		meta::foreach(dst, src, move_value{});
	}

	void create_n(size_type size) { meta::foreach(data_, construct_data{size}); }
	void destroy_n(size_type size) noexcept { meta::foreach(data_, destroy_data{size}); }
	void copy_n(const table& other, size_type size) noexcept { meta::foreach(data_, other.data_, copy_data{size}); }
	void move_n(const table& other, size_type size) noexcept { meta::foreach(data_, other.data_, move_data{size}); }

	void copy_pointer(const table& other) noexcept { meta::foreach(data_, other.data_, set_pointer{}); }

private:
	pointers_type data_;

public:
	static inline constexpr size_t memory_size(size_t rows_count) noexcept { return rows_count * row_sizeof; }

	static constexpr size_t row_sizeof = meta::list_sizeof<TypesList>();
};
} // namespace data_table
} // namespace db
} // namespace A3D

#endif // CONTAINER_META_DATABASE_TABLE_H
