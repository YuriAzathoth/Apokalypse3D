/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2023 Yuriy Zinchenko

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

#ifndef CONTAINER_STRING_HASH_H
#define CONTAINER_STRING_HASH_H

#include <stdint.h>
#include "string.h"

namespace std
{
template <>
struct hash<const char*>
{
	constexpr size_t operator()(const char* str) const noexcept
	{
		size_t hash = 0;
		while (*str)
		{
			hash += (size_t)*str;
			hash += (hash << 10Lu);
			hash ^= (hash << 6Lu);
			++str;
		}
		hash += (hash << 3Lu);
		hash ^= (hash << 11Lu);
		hash += (hash << 15Lu);
		return hash;
	}
};
}

namespace A3D
{
template <typename String, typename Hasher = std::hash<String>>
class basic_string_hash
{
public:
	using value_type = size_t;
	using string_type = String;
	using hasher_type = Hasher;

	constexpr basic_string_hash() noexcept :
		hash_(0)
	{
	}

	constexpr basic_string_hash(const string_type& str) noexcept :
		hash_(hasher_type{}(str))
	{
	}

	constexpr basic_string_hash(const char* str) noexcept :
		hash_(std::hash<const char*>{}(str))
	{
	}

	constexpr basic_string_hash(const basic_string_hash& other) noexcept :
		hash_(other.hash_)
	{
	}

	constexpr void operator=(const string_type& str) noexcept
	{
		hash_ = hasher_type{}(str);
	}

	constexpr void operator=(const char* str) noexcept
	{
		hash_ = std::hash<const char*>{}(str);
	}

	constexpr void operator=(const basic_string_hash& other) noexcept
	{
		hash_ = other.hash_;
	}

	constexpr bool operator==(const basic_string_hash& other) const noexcept
	{
		return hash_ == other.hash_;
	}

	constexpr bool operator!=(const basic_string_hash& other) const noexcept
	{
		return hash_ != other.hash_;
	}

	constexpr value_type hash() const noexcept
	{
		return hash_;
	}

private:
	value_type hash_;
};

using string_hash = basic_string_hash<string>;
} // namespace A3D

namespace std
{
template <typename String, typename Hasher>
struct hash<A3D::basic_string_hash<String, Hasher>>
{
	size_t operator()(A3D::basic_string_hash<String, Hasher> str) const noexcept
	{
		return str.hash();
	}
};
}

#endif // CONTAINER_STRING_HASH_H
