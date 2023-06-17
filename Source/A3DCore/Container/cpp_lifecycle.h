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

#ifndef CONTAINER_CPP_LIFECYCLE_H
#define CONTAINER_CPP_LIFECYCLE_H

#include <string.h>
#include <memory>
#include <type_traits>
#include <utility>

namespace A3D
{
template <typename T, typename... Args>
inline void construct(T* ptr, Args&&... args)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		std::construct_at(ptr, std::forward<Args>(args)...);
	else
		*ptr = T(std::forward<Args>(args)...);
};

template <typename T>
inline void destroy(T* ptr)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		std::destroy_at(ptr);
};

template <typename T>
inline void copy_construct(T* dst, const T* src)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		std::construct_at(dst, *src);
	else
		*dst = *src;
};

template <typename T>
inline void move_construct(T* dst, T* src)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		std::construct_at(dst, std::move(*src));
	else
		*dst = *src;
};

template <typename T>
inline void copy_assign(T* dst, const T* src)
{
	*dst = *src;
};

template <typename T>
inline void move_assign(T* dst, T* src)
{
	*dst = std::move(*src);
};

template <typename T, typename... Args>
inline void construct_n(T* begin, const T* end, const Args&... args)
{
	while (begin < end)
	{
		construct(begin);
		++begin;
	}
};

template <typename T, typename... Args>
inline void construct_n(T* begin, size_t size, const Args&... args)
{
	construct_n(begin, begin + size, args...);
};

template <typename T>
inline void destroy_n(T* begin, const T* end)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		while (begin < end)
		{
			destroy(begin);
			++begin;
		}
};

template <typename T>
inline void destroy_n(T* begin, size_t size)
{
	destroy_n(begin, begin + size);
};

template <typename T>
inline void copy_construct_n(T* dst, const T* src, size_t size)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		for (const T* dst_end = dst + size; dst < dst_end; ++dst, ++src)
			copy_construct(dst, src);
	else
		memcpy(dst, src, size * sizeof(T));
};

template <typename T>
inline void copy_construct_n(T* dst, const T* src, const T* src_end)
{
	copy_construct_n(dst, src, src_end - src);
};

template <typename T>
inline void move_construct_n(T* dst, T* src, size_t size)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		for (const T* dst_end = dst + size; dst < dst_end; ++dst, ++src)
			move_construct(dst, src);
	else
		memcpy(dst, src, size * sizeof(T));
};

template <typename T>
inline void move_construct_n(T* dst, T* src, const T* src_end)
{
	move_construct_n(dst, src, src_end);
};

template <typename T>
inline void copy_assign_n(T* dst, const T* src, size_t size)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		for (const T* dst_end = dst + size; dst < dst_end; ++dst, ++src)
			copy_assign(dst, src);
	else
		memcpy(dst, src, size * sizeof(T));
}

template <typename T>
inline void copy_assign_n(T* dst, const T* src, const T* src_end)
{
	copy_assign_n(dst, src, src_end - src);
}

template <typename T>
inline void move_assign_n(T* dst, T* src, size_t size)
{
	if constexpr (!std::is_integral<T>::type && !std::is_trivial<T>::type)
		for (const T* dst_end = dst + size; dst < dst_end; ++dst, ++src)
			move_assign(dst, src);
	else
		memcpy(dst, src, size * sizeof(T));
}

template <typename T>
inline void move_assign_n(T* dst, T* src, const T* src_end)
{
	move_assign_n(dst, src, src_end - src);
}
} // namespace A3D

#endif // CONTAINER_CPP_LIFECYCLE_H
