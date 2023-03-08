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

#ifndef CONTAINER_DETAIL_NOEXCEPT_ALLOCATOR_H
#define CONTAINER_DETAIL_NOEXCEPT_ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>
#include <memory>
#include <type_traits>

namespace A3D
{
namespace detail
{
template <typename T>
struct noexcept_allocator
{
	using value_type = T;
	using size_type = size_t;
	using difference_type = ptrdiff_t;
	using propagate_on_container_move_assignment = std::true_type;

	constexpr noexcept_allocator() noexcept = default;
	constexpr noexcept_allocator(const noexcept_allocator& other) noexcept = default;
	template <typename U> constexpr noexcept_allocator(const noexcept_allocator<U>& other) noexcept {};
	constexpr ~noexcept_allocator() {}

	[[nodiscard]] constexpr T* allocate(size_t n) noexcept
	{
		T* ret = reinterpret_cast<T*>(malloc(n * sizeof(T)));
		if (ret != nullptr)
			return ret;
		else
		{
			std::terminate();
			return nullptr;
		}
	}

	constexpr void deallocate(T* p, [[maybe_unused]] size_t n) noexcept
	{
		free(p);
	}
};
} // namespace detail
} // namespace A3D

#endif // CONTAINER_DETAIL_NOEXCEPT_ALLOCATOR_H
