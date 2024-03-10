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

#ifndef CONTAINER_META_DATABASE_H
#define CONTAINER_META_DATABASE_H

#include <stdint.h>
#include <string.h>
#include <bit>
#include <limits>
#include <memory>
#include <utility>
#include "database_table.h"
#include "types_list.h"

namespace A3D
{
namespace db
{
struct primary_key {};

template <typename SizeType,
		  typename ChunkType,
		  typename DataTypesList,
		  typename TagsTypesList,
		  typename Allocator = std::allocator<uint8_t>>
class database
{
public:
	using primary_index = SizeType;
	using internal_index = SizeType;
	using size_type = SizeType;
	using data_types = DataTypesList;
	using data_types_list = meta::make_data_list<data_types>;
	using tags_types = TagsTypesList;
	using data_table = table::table<data_types_list>;
	using chunk_type = ChunkType;
	using primary_indices = pkey::map<size_type, chunk_type>;
	using allocator_type = Allocator;

	enum { chunk_size = sizeof(size_type) * 8 };

	template <typename... Tags>
	using iterator = table::iterator<data_table, tags_types, typename meta::types_list_builder<Tags...>::type>;

	template <typename... Tags>
	using const_iterator = table::const_iterator<data_table, tags_types, typename meta::types_list_builder<Tags...>::type>;

	using iterator_all = table::iterator<data_table, tags_types, tags_types>;
	using const_iterator_all = table::const_iterator<data_table, tags_types, tags_types>;

	database() :
		size_(0),
		capacity_(0)
	{
	}

	~database()
	{
		release();
	}

	bool empty() const noexcept { return size_ == 0; }
	size_type capacity() const noexcept { return capacity_; }
	size_type size() const noexcept { return size_; }

	template <typename... Tags>
	iterator<Tags...> begin() noexcept
	{
		return iterator<Tags...>(data_);
	}

	template <typename... Tags>
	const_iterator<Tags...> begin() const noexcept
	{
		return const_iterator<Tags...>(data_);
	}

	template <typename... Tags>
	const_iterator<Tags...> cbegin() const noexcept
	{
		return const_iterator<Tags...>(data_);
	}

	template <typename... Tags>
	iterator<Tags...> end() noexcept
	{
		return iterator<Tags...>(data_, size_);
	}

	template <typename... Tags>
	const_iterator<Tags...> end() const noexcept
	{
		return const_iterator<Tags...>(data_, size_);
	}

	template <typename... Tags>
	const_iterator<Tags...> cend() const noexcept
	{
		return const_iterator<Tags...>(data_, size_);
	}

	iterator_all abegin() noexcept { return iterator_all(data_); }
	const_iterator_all abegin() const noexcept { return const_iterator_all(data_); }
	const_iterator_all cabegin() const noexcept { return const_iterator_all(data_); }

	iterator_all aend() noexcept { return iterator_all(data_, size_); }
	const_iterator_all aend() const noexcept { return const_iterator_all(data_, size_); }
	const_iterator_all caend() const noexcept { return const_iterator_all(data_, size_); }

	template <typename Tag>
	typename meta::type_by_tag<Tag, tags_types, data_types>::type&
	front() noexcept
	{
		return *meta::get_tag<Tag, tags_types>(data_);
	}

	template <typename Tag>
	const typename meta::type_by_tag<Tag, tags_types, data_types>::type&
	front() const noexcept
	{
		return *meta::get_tag<Tag, tags_types>(data_);
	}

	template <typename Tag>
	typename meta::type_by_tag<Tag, tags_types, data_types>::type&
	back() noexcept
	{
		return *meta::get_tag<Tag, tags_types>(end().ptr()) - 1;
	}

	template <typename Tag>
	const typename meta::type_by_tag<Tag, tags_types, data_types>::type&
	back() const noexcept
	{
		return *meta::get_tag<Tag, tags_types>(end().ptr()) - 1;
	}

	std::pair<primary_index, iterator_all> insert()
	{
		if (size_ + 1 > capacity_)
			data_add_chunk();

		const primary_index key = size_;

		iterator_all it = aend();
		table::construct(it);

		return std::make_pair<primary_index, iterator_all>(key, it);
	}

	void erase(primary_index key)
	{
	}

private:
	void release()
	{
		if (capacity_ > 0)
		{
			if (size_ > 0)
				table::destroy_n(data_, size_);
		}
	}

	void data_add_chunk()
	{
		if (capacity_ > 0)
		{
			data_table new_data;
			table::allocate(new_data, alloc_, capacity_ + chunk_size);
			if (size_ > 0)
				table::move_construct(new_data, data_, size_);
			table::destruct(data_, size_);
			table::deallocate(data_, alloc_, capacity_);
			table::set_pointer(data_, new_data);
			capacity_ += chunk_size;
		}
		else
		{
			table::allocate(data_, alloc_, chunk_size);
			capacity_ = chunk_size;
		}
	}

	void data_shrink()
	{
		if (size_ > 0)
		{
			data_table new_data;
			table::allocate(new_data, alloc_, size_);
			table::move_construct(new_data, data_, size_);
			table::destruct(data_, size_);
			table::deallocate(data_, alloc_, capacity_);
			table::set_pointer(data_, new_data);
		}
		else
		{
			table::destruct(data_, size_);
			table::deallocate(data_, alloc_, capacity_);
		}
		capacity_ = size_;
	}

	internal_index* primary_keys_;
	chunk_type* states_;
	data_table data_;
	size_type size_;
	size_type capacity_;
	allocator_type alloc_;
};
} // namespace db
} // namespace A3D

#endif // CONTAINER_META_DATABASE_H
