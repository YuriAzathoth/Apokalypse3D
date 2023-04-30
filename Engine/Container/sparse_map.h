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

#ifndef CONTAINER_SPARSE_MAP_H
#define CONTAINER_SPARSE_MAP_H

#include <stdint.h>
#include <string.h>
#include <bit>
#include <limits>
#include <memory>
#include <type_traits>
#include "noexcept_allocator.h"

namespace A3D
{
template <typename Key,
		  typename Value,
		  typename Bitfield = uint32_t,
		  typename Allocator = noexcept_allocator<Value>>
class sparse_map
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
	using bitfield_type = Bitfield;

	static constexpr key_type INVALID_KEY = std::numeric_limits<key_type>::max();

	sparse_map() :
		size_(0),
		capacity_(0)
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_trivial<Value>::value);
		static_assert(std::is_integral<Key>::value);
	}

	explicit sparse_map(const Allocator& alloc) :
		size_(0),
		capacity_(0),
		alloc_(alloc)
	{
		static_assert(std::is_trivial<Key>::value);
		static_assert(std::is_trivial<Value>::value);
		static_assert(std::is_integral<Key>::value);
	}

	sparse_map(const sparse_map& other) :
		size_(other.size_),
		capacity_(other.capacity_),
		alloc_(other.alloc_)
	{
		const size_type bitfield_size = allocate(data_, items_state_, capacity_);
		memcpy(data_, other.data_, capacity_ * sizeof(value_type));
		memcpy(items_state_, other.items_state_, bitfield_size);
	}

	sparse_map(sparse_map&& other) noexcept :
		data_(other.data_),
		items_state_(other.items_state_),
		size_(other.size_),
		capacity_(other.capacity_),
		alloc_(other.alloc_)
	{
		other.size_ = 0;
		other.capacity_ = 0;
	}

	~sparse_map() { destroy(); }

	void operator=(const sparse_map& other)
	{
		destroy();

		size_ = other.size_;
		capacity_ = other.capacity_;

		const size_type bitfield_size = allocate(data_, items_state_, capacity_);
		memcpy(data_, other.data_, capacity_ * sizeof(value_type));
		memcpy(items_state_, other.items_state_, bitfield_size);
	}

	void operator=(sparse_map&& other) noexcept
	{
		destroy();

		data_ = other.data_;
		items_state_ = other.items_state_;
		size_ = other.size_;
		capacity_ = other.capacity_;

		other.size_ = 0;
		other.capacity_ = 0;
	}

	value_type& operator[](key_type key) noexcept { return data_[key]; }
	const value_type& operator[](key_type key) const noexcept { return data_[key]; }

	size_type capacity() const noexcept { return capacity_; }
	size_type size() const noexcept { return size_; }
	[[nodiscard]] bool empty() const noexcept { return size_ == 0; }

	bool contains(key_type key) const noexcept
	{
		const size_type segment = get_bf_segment(key);
		const bitfield_type bit = get_bf_bit(key);
		return capacity_ > 0 && get_bf_value(items_state_[segment], bit);
	}

	key_type insert(value_type value)
	{
		if (size_ + 1 > capacity_)
			if (!reserve(capacity_ + BITS_IN_BITFIELD))
				return INVALID_KEY;

		++size_;

		const key_type id = take_empty_cell();
		if (id != INVALID_KEY)
			data_[id] = value;

		return id;
	}

	void erase(key_type key)
	{
		const size_type segment = get_bf_segment(key);
		const bitfield_type bit = get_bf_bit(key);
		disable_bf_bit(items_state_[segment], bit);

		--size_;
	}

	void clear()
	{
		destroy();
		size_ = 0;
		capacity_ = 0;
	}

	bool reserve(size_type count)
	{
		pointer new_data;
		bitfield_type* new_items_state;
		const size_type bitfield_count = allocate(new_data, new_items_state, count);
		if (bitfield_count > 0)
		{
			const size_type old_data_size = capacity_ * sizeof(value_type);
			const size_type old_items_state_size = get_bitfield_size(capacity_) * sizeof(bitfield_type);
			const size_type new_data_size = count * sizeof(value_type);
			const size_type new_items_state_size = bitfield_count * sizeof(bitfield_type);

			if (capacity_ > 0)
			{
				memcpy(new_data, data_, old_data_size);
				memcpy(new_items_state, items_state_, old_items_state_size);
				deallocate(data_, items_state_);
			}

			data_ = new_data;
			items_state_ = new_items_state;
			capacity_ = count;

			return true;
		}
		else
			return false;
	}

	bool shrink_to_fit()
	{
		size_type to_deallocate = get_segments_to_shrink();
		if (to_deallocate == 0)
			return false;

		const size_type new_capacity = capacity_ - to_deallocate * BITS_IN_BITFIELD;
		if (new_capacity > 0)
		{
			pointer new_data;
			bitfield_type* new_items_state;
			const size_type new_items_state_size = allocate(new_data, new_items_state, new_capacity);
			if (new_items_state_size == 0)
				return false;

			if (capacity_)
			{
				memcpy(new_data, data_, capacity_ * sizeof(value_type));
				memcpy(new_items_state, items_state_, new_items_state_size);
				deallocate(data_, items_state_);
			}

			data_ = new_data;
			items_state_ = new_items_state;
		}
		else
			deallocate(data_, items_state_);

		capacity_ = new_capacity;

		return true;
	}

	size_t memory_size() const noexcept
	{
		return sizeof(sparse_map) + capacity_ * sizeof(value_type) + get_bitfield_size(capacity_) * sizeof(bitfield_type);
	}

#ifndef NDEBUG
	void debug_print() const
	{
		printf("Sparse Map\n");
		printf("  Size:     %u\n", size_);
		printf("  Capacity: %u\n", capacity_);
		for (key_type i = 0; i < capacity_; ++i)
		{
			printf("    %u[%u, %u -> %u]:\t", i, get_bf_segment(i), get_bf_bit_position(i), get_bf_bit(i));
			if (contains(i))
				printf("%u\n", data_[i]);
			else
				printf("Empty\n");
		}
	}
#endif // NDEBUG

private:
	static constexpr bitfield_type BITS_IN_BITFIELD = static_cast<bitfield_type>(sizeof(bitfield_type) * 8); // 4 * 8 = 32
	static constexpr bitfield_type BITS_ONE = static_cast<bitfield_type>(1);
	static constexpr bitfield_type BITS_ALL_DISABLED = static_cast<bitfield_type>(0);
	static constexpr bitfield_type BITS_ALL_ENABLED = ~BITS_ALL_DISABLED;
	static constexpr key_type BITFIELD_MAX_VALUE_BITS = std::bit_width(BITS_IN_BITFIELD - 1);

	size_type allocate(pointer& data, bitfield_type*& items_state, size_type capacity)
	{
		using rebound_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<uint8_t>;
		rebound_allocator_type rebound_allocator(alloc_);

		const size_type data_size = capacity * sizeof(value_type);
		const size_type bitfield_size = get_bitfield_size(capacity) * sizeof(bitfield_type);
		uint8_t* memory = rebound_allocator.allocate(data_size + bitfield_size);
		data = reinterpret_cast<pointer>(memory);
		memory += data_size;
		items_state = reinterpret_cast<bitfield_type*>(memory);
		if (memory != nullptr)
		{
			memset(items_state, 0, bitfield_size);
			return bitfield_size;
		}
		else
			return 0;
	}

	void deallocate(pointer data, [[maybe_unused]] bitfield_type* items_state)
	{
		using rebound_allocator_type = typename std::allocator_traits<allocator_type>::template rebind_alloc<uint8_t>;
		rebound_allocator_type rebound_allocator(alloc_);

		const size_type size = capacity_ * sizeof(value_type) + get_bitfield_size(capacity_) * sizeof(bitfield_type);
		rebound_allocator.deallocate(reinterpret_cast<uint8_t*>(data), size);
	}

	void destroy()
	{
		if (capacity_ > 0)
			deallocate(data_, items_state_);
	}

	key_type take_empty_cell()
	{
		const size_type bitfield_count = get_bitfield_size(capacity_);
		for (size_type i = 0; i < bitfield_count; ++i)
		{
			if (is_bf_segment_not_full(items_state_[i]))
			{
				const bitfield_type bit = get_bf_first_empty_bit(items_state_[i]);
				enable_bf_bit(items_state_[i], bit);
				return build_bf_key(i, bit);
			}
		}
		return INVALID_KEY;
	}

	size_type get_segments_to_shrink() const noexcept
	{
		size_type ret = 0;
		size_type segment = get_bitfield_size(capacity_) - 1;
		while (items_state_[segment] == BITS_ALL_DISABLED)
		{
			++ret;
			if (segment == 0)
				break;
			--segment;
		}
		return ret;
	}

	static size_type get_bf_segment(key_type key) noexcept
	{
		return static_cast<size_type>(key >> BITFIELD_MAX_VALUE_BITS);
	}

	static bitfield_type get_bf_bit_position(key_type key) noexcept
	{
		return static_cast<bitfield_type>(key) & (BITS_IN_BITFIELD - 1);
	}

	static bitfield_type get_bf_bit(key_type key) noexcept
	{
		return BITS_ONE << get_bf_bit_position(key);
	}

	static size_type get_bf_first_empty_bit(bitfield_type bitfield) noexcept
	{
		return static_cast<size_type>(get_bf_bit(std::countr_one(bitfield)));
	}

	static bool get_bf_value(bitfield_type bitfield, bitfield_type bit) noexcept
	{
		return bitfield & bit;
	}

	static bool is_bf_segment_not_full(bitfield_type bitfield) noexcept
	{
		return bitfield != BITS_ALL_ENABLED;
	}

	static void enable_bf_bit(bitfield_type& bitfield, bitfield_type bit) noexcept
	{
		bitfield |= bit;
	}

	static void disable_bf_bit(bitfield_type& bitfield, bitfield_type bit) noexcept
	{
		bitfield &= ~bit;
	}

	static size_type build_bf_key(size_type segment, bitfield_type bit) noexcept
	{
		return (segment << BITFIELD_MAX_VALUE_BITS) | std::countr_zero(bit);
	}

	static size_type get_bitfield_size(size_type capacity) noexcept
	{
		return capacity >> BITFIELD_MAX_VALUE_BITS;
	}

	pointer data_;
	bitfield_type* items_state_;
	size_type size_;
	size_type capacity_;
	allocator_type alloc_;
};
} // namespace A3D

#endif // CONTAINER_SPARSE_MAP_H
