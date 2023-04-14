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

#ifndef CONTAINER_DENSE_MAP_H
#define CONTAINER_DENSE_MAP_H

#include <stdint.h>
#include <string.h>
#include <bit>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include "noexcept_allocator.h"

namespace A3D
{
template <typename Key,
		  typename Value,
		  typename Allocator = noexcept_allocator<Value>>
class dense_map
{
public:
	using key_type = Key;
	using value_type = Value;
	using size_type = key_type;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;
	using allocator_type = Allocator;
	using iterator = pointer;
	using const_iterator = const_pointer;

	static constexpr key_type INVALID_KEY = std::numeric_limits<key_type>::max();

	dense_map() :
		size_(0),
		capacity_(0)
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_integral<Key>::value);
	}

	explicit dense_map(const Allocator& alloc) :
		size_(0),
		capacity_(0),
		alloc_(alloc)
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_integral<Key>::value);
	}

	dense_map(const dense_map& other) :
		size_(other.size_),
		capacity_(other.capacity_),
		alloc_(other.alloc_)
	{
		data_ = alloc_.allocate(capacity_);

		if constexpr (std::is_trivial<value_type>::value)
			memcpy(data_, other.data_, capacity_ * sizeof(value_type));
		else
		{
			const_iterator src_it = other.begin();
			const const_iterator src_end = other.end();
			iterator dst_it = begin();
			for (; src_it != src_end; ++src_it, ++dst_it)
				std::construct_at(dst_it, *src_it);
		}
	}

	dense_map(dense_map&& other) noexcept :
		data_(other.data_),
		size_(other.size_),
		capacity_(other.capacity_),
		alloc_(other.alloc_)
	{
		other.size_ = 0;
		other.capacity_ = 0;
	}

	~dense_map() { destroy(); }

	void operator=(const dense_map& other)
	{
		destroy();

		size_ = other.size_;
		capacity_ = other.capacity_;
		data_ = alloc_.allocate(capacity_);

		if constexpr (std::is_trivial<value_type>::value)
			memcpy(data_, other.data_, other.capacity_ * sizeof(value_type));
		else
		{
			const_iterator src_it = other.begin();
			const const_iterator src_end = other.end();
			iterator dst_it = begin();
			for (; src_it != src_end; ++src_it, ++dst_it)
				std::construct_at(dst_it, *src_it);
		}
	}

	void operator=(dense_map&& other) noexcept
	{
		destroy();

		data_ = other.data_;
		size_ = other.size_;
		capacity_ = other.capacity_;
		other.size_ = 0;
		other.capacity_ = 0;
	}

	iterator begin() noexcept { return data_; }
	const_iterator begin() const noexcept { return data_; }
	const_iterator cbegin() const noexcept { return data_; }
	iterator end() noexcept { return data_ + size_; }
	const_iterator end() const noexcept { return data_ + size_; }
	const_iterator cend() const noexcept { return data_ + size_; }

	value_type& front() noexcept { return data_[0]; }
	const value_type& front() const noexcept { return data_[0]; }
	value_type& back() noexcept { return data_[size_ - 1]; }
	const value_type& back() const noexcept { return data_[size_ - 1]; }

	value_type& operator[](key_type key) noexcept { return data_[key]; }
	const value_type& operator[](key_type key) const noexcept { return data_[key]; }

	pointer data() noexcept { return data_; }
	const_pointer data() const noexcept { return data_; }

	size_type size() const noexcept { return size_; }
	size_type capacity() const noexcept { return capacity_; }
	[[nodiscard]] bool empty() const noexcept { return size_ == 0; }

	template <typename... Args>
	key_type emplace_back(Args&&... args)
	{
		if (size_ + 1 > capacity_)
			if (!reserve(capacity_ + GROW_FACTOR))
				return INVALID_KEY;

		const key_type key = size_;
		if constexpr (std::is_trivial<value_type>::value)
			data_[size_] = { std::forward<Args>(args)... };
		else
			std::construct_at(&data_[size_], std::forward<Args>(args)...);

		++size_;

		return size_;
	}

	key_type push_back(const value_type& value)
	{
		return emplace_back(value);
	}

	key_type push_back(value_type&& value)
	{
		return emplace_back(std::move(value));
	}

	void pop_back()
	{
		--size_;

		if constexpr (!std::is_trivial<value_type>::value)
			std::destroy_at(&data_[size_]);
	}

	key_type erase_swap(key_type key)
	{
		const key_type last_key = size_ - 1;
		const iterator it = data_ + key;
		const iterator last = data_ + last_key;

		if constexpr (std::is_trivial<value_type>::value)
			*it = *last;
		else
			*it = std::move(*last);

		return last_key;
	}

	void clear()
	{
		destroy();

		size_ = 0;
		capacity_ = 0;
	}

	bool reserve(size_type count)
	{
		pointer new_data = alloc_.allocate(count);
		if (new_data == nullptr)
			return false;

		if (capacity_ > 0)
		{
			if constexpr (std::is_trivial<value_type>::value)
				memcpy(new_data, data_, capacity_ * sizeof(value_type));
			else
			{
				const_iterator src_it = begin();
				const const_iterator src_end = end();
				pointer dst_ptr = new_data;
				for (; src_it != src_end; ++src_it, ++dst_ptr)
					std::construct_at(dst_ptr, std::move(*src_it));
			}
		}

		data_ = new_data;
		capacity_ = count;

		return true;
	}

	bool shrink_to_fit()
	{
		if (size_ > 0)
		{
			pointer new_data = alloc_.allocate(size_);
			if (new_data == nullptr)
				return false;

			if (capacity_ > 0)
			{
				if constexpr (std::is_trivial<value_type>::value)
					memcpy(new_data, data_, size_ * sizeof(value_type));
				else
				{
					const_iterator src_it = begin();
					pointer dst_ptr = new_data;
					const pointer dst_end = new_data + size;
					for (; dst_ptr != dst_end; ++src_it, ++dst_ptr)
						std::construct_at(dst_ptr, std::move(*src_it));

					const const_iterator src_end = end();
					for (; src_it != src_end; ++src_it)
						std::destroy_at(src_it);
				}
			}

			data_ = new_data;
		}

		capacity_ = size_;

		return true;
	}

	size_t memory_size() const noexcept
	{
		return sizeof(dense_map) + capacity_ * sizeof(value_type);
	}

#ifndef NDEBUG
	void debug_print() const
	{
		printf("Dense Map\n");
		printf("  Size:     %u\n", size_);
		printf("  Capacity: %u\n", capacity_);
		for (key_type i = 0; i < size_; ++i)
			printf("[%u]:\t%u\n", i, data_[i]);
	}
#endif // NDEBUG

private:
	static constexpr size_type GROW_FACTOR = 16;

	void destroy()
	{
		if (capacity_)
		{
			if constexpr (!std::is_trivial<value_type>::value)
				for (iterator it = begin(); it != end(); ++it)
					std::destroy_at(it);

			alloc_.deallocate(data_, capacity_);
		}
	}

	pointer data_;
	size_type size_;
	size_type capacity_;
	allocator_type alloc_;
};
} // namespace A3D

#endif // CONTAINER_DENSE_MAP_H
