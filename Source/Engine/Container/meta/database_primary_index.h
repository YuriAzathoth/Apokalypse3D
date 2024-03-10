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

#ifndef CONTAINER_META_DATABASE_PRIMARY_INDEX_H
#define CONTAINER_META_DATABASE_PRIMARY_INDEX_H

#include <stdint.h>
#include <string.h>
#include <bit>
#include <limits>
#include "types_list.h"

namespace A3D
{
namespace db
{
namespace primary_index
{
template <typename SizeType> struct narrower;
template <> struct narrower <uint16_t> { using type = uint8_t; };
template <> struct narrower <uint32_t> { using type = uint16_t; };
template <> struct narrower <uint64_t>{ using type = uint32_t; };

template <typename SizeType, typename StateBitfield>
class index
{
public:
	using value_type = SizeType;
	using states_bitfield = StateBitfield;
	using size_type = SizeType;
	using key_type = SizeType;
	using chunk_id_type = typename narrower<key_type>::type;
	using bit_id_type = typename narrower<key_type>::type;

	static constexpr states_bitfield chunk_empty = static_cast<states_bitfield>(0);
	static constexpr states_bitfield chunk_full = std::numeric_limits<states_bitfield>::max();
	static constexpr key_type invalid_key = std::numeric_limits<key_type>::max();
	static constexpr key_type chunk_size = 8 * sizeof(states_bitfield);
	static constexpr key_type bits_mask = chunk_size - 1;
	static constexpr key_type bits_shift = std::countr_one<states_bitfield>(bits_mask);

	static constexpr chunk_id_type get_chunk_id(key_type key) noexcept
	{
		return static_cast<chunk_id_type>(key >> bits_shift);
	}

	static constexpr bit_id_type get_bit_id(key_type key) noexcept
	{
		return static_cast<bit_id_type>(key & bits_mask);
	}

	static constexpr key_type get_key(chunk_id_type chunk_id, bit_id_type bit_id) noexcept
	{
		return static_cast<key_type>(chunk_id << bits_shift) | static_cast<key_type>(bit_id);;
	}

	static constexpr size_t states_count(size_type rows_count) noexcept
	{
		return rows_count >> bits_shift;
	}

	static constexpr size_t indices_size(size_type rows_count) noexcept
	{
		return rows_count * sizeof(value_type);
	}

	static constexpr size_t states_size(size_type rows_count) noexcept
	{
		return (rows_count * sizeof(states_bitfield)) >> bits_shift;
	}

	uint8_t* allocate(uint8_t* mem, size_t indices_size, size_t states_size) noexcept
	{
		indices_ = reinterpret_cast<value_type*>(mem);
		mem += indices_size;
		states_ = reinterpret_cast<states_bitfield*>(mem);
		return mem + states_size;
	}

	void copy(const index& src, size_type size) noexcept
	{
		memcpy(states_, src.states_, size);
	}

	void copy_ptr(const index& other) noexcept
	{
		states_ = other.states_;
	}

	bool contains(key_type key) const noexcept
	{
		return states_[get_chunk_id(key)] & (static_cast<states_bitfield>(1) << static_cast<states_bitfield>(get_bit_id(key)));
	}

	key_type insert(value_type value, size_type states_count) noexcept
	{
		for (states_bitfield* state = states_; state < states_ + states_count; ++state)
			if (*state != chunk_full)
			{
				const key_type key = get_key(static_cast<chunk_id_type>(state - states_),
											 std::countr_one<states_bitfield>(*state));
				indices_[key] = value;
				set_bit(key);
				return key;
			}
		return invalid_key;
	}

	void erase(key_type key) noexcept
	{
		reset_bit(key);
	}

	void clear(size_type states_count) noexcept
	{
		for (states_bitfield* state = states_; state < states_ + states_count; ++state)
			*state = chunk_empty;
	}

	value_type& operator[](key_type key) noexcept
	{
		return indices_[key];
	}

	const value_type& operator[](key_type key) const noexcept
	{
		return indices_[key];
	}

	const states_bitfield* data() const noexcept { return states_; }

private:
	void set_bit(key_type key) noexcept
	{
		states_[get_chunk_id(key)] |= (static_cast<states_bitfield>(1) << static_cast<states_bitfield>(get_bit_id(key)));
	}

	void reset_bit(key_type key) noexcept
	{
		states_[get_chunk_id(key)] &= ~(static_cast<states_bitfield>(1) << static_cast<states_bitfield>(get_bit_id(key)));
	}

	value_type* indices_;
	states_bitfield* states_;
};
} // namespace primary_index
} // namespace db
} // namespace A3D

#endif // CONTAINER_META_DATABASE_PRIMARY_INDEX_H
