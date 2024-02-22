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

#ifndef CONTAINER_DENSE_MAP_H
#define CONTAINER_DENSE_MAP_H

#include <stdint.h>
#include <string.h>
#include <limits>
#include <memory>
#include <type_traits>
#include <utility>
#include "noexcept_allocator.h"
#include "vector.h"

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

	dense_map()
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_integral<Key>::value);
	}

	explicit dense_map(const Allocator& alloc) :
		data_(alloc)
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_integral<Key>::value);
	}

	dense_map(const dense_map& other) : data_(other.data_) { }
	dense_map(dense_map&& other) noexcept : data_(std::move(other.data_)) {}

	void operator=(const dense_map& other) { data_ = other.data_; }
	void operator=(dense_map&& other) noexcept { data_ = std::move(other.data_); }

	iterator begin() noexcept { return data_.begin(); }
	const_iterator begin() const noexcept { return data_.begin(); }
	const_iterator cbegin() const noexcept { return data_.cbegin(); }
	iterator end() noexcept { return data_.end(); }
	const_iterator end() const noexcept { return data_.end(); }
	const_iterator cend() const noexcept { return data_.cend(); }

	value_type& front() noexcept { return data_.front(); }
	const value_type& front() const noexcept { return data_.front(); }
	value_type& back() noexcept { return data_.back(); }
	const value_type& back() const noexcept { return data_.back(); }

	value_type& operator[](key_type key) noexcept { return data_[key]; }
	const value_type& operator[](key_type key) const noexcept { return data_[key]; }

	pointer data() noexcept { return data_.data(); }
	const_pointer data() const noexcept { return data_.data(); }

	size_type size() const noexcept { return data_.size(); }
	size_type capacity() const noexcept { return data_.capacity(); }
	[[nodiscard]] bool empty() const noexcept { return data_.empty(); }

	template <typename... Args>
	key_type emplace(Args&&... args)
	{
		const key_type key = static_cast<key_type>(data_.size());
		const bool res = data_.emplace_back(std::forward<Args>(args)...);
		return res ? key : INVALID_KEY;
	}

	key_type insert(const value_type& value) { return emplace(value); }
	key_type insert(value_type&& value) { return emplace(std::move(value)); }

	key_type erase(key_type key) { return erase(data_.begin() + key); }
	key_type erase(const_iterator it) { return erase(const_cast<iterator>(it)); }

	key_type erase(iterator it) noexcept
	{
		const iterator last = data_.end() - 1;
		const bool is_not_last = it < last;
		if (is_not_last)
			move(it, last);

		data_.pop_back();

		return is_not_last ? last - data_.begin() : INVALID_KEY;
	}

	void move(key_type dst, key_type src) noexcept { move(begin() + dst, begin() + src); }

	static void move(const_iterator dst, const_iterator src) noexcept
	{
		move(const_cast<iterator>(dst), const_cast<iterator>(src));
	}

	static void move(iterator dst, iterator src) noexcept
	{
		if constexpr (std::is_trivial<value_type>::value)
			*dst = *src;
		else
			*dst = std::move(*src);
	}

	void swap_items(key_type left, key_type right) noexcept
	{
		swap_items(begin() + left, begin() + right);
	}

	static void swap_items(const_iterator left, const_iterator right) noexcept
	{
		swap_items(const_cast<iterator>(left), const_cast<iterator>(right));
	}

	static void swap_items(iterator left, iterator right) noexcept
	{
		value_type temp;
		move(&temp, left);
		move(left, right);
		move(right, &temp);
	}

	void clear() { data_.clear(); }

	bool reserve(size_type count) { return data_.reserve(count); }
	bool shrink_to_fit() { return data_.shrink_to_fit(); }

	size_t memory_size() const noexcept { return data_.memory_size(); }

private:
	vector<key_type, value_type, allocator_type> data_;
};
} // namespace A3D

#endif // CONTAINER_DENSE_MAP_H
