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

#ifndef CONTAINER_DETAIL_MEMORY_DEBUG_ALLOCATOR_H
#define CONTAINER_DETAIL_MEMORY_DEBUG_ALLOCATOR_H

#include <stdint.h>
#include <stdio.h>
#include <mutex>
#include <string>
#include <unordered_set>
#include "noexcept_allocator.h"

namespace A3D
{
namespace detail
{
class memory_debugger
{
public:
	memory_debugger() = default;
	memory_debugger(memory_debugger&& other) noexcept : records_(std::move(other.records_)) {}
	void operator=(memory_debugger&& other) noexcept { records_ = std::move(other.records_); }

	void check_leaks() const
	{
		if (!records_.empty())
		{
			fprintf(stderr, "%u memory leaks detected: allocate without deallocation.", (unsigned)records_.size());
			std::terminate();
		}
	}

	void do_allocation(void* p, size_t n)
	{
		std::lock_guard lock(mutex_);
		if (n == 0)
		{
			fprintf(stderr, "Failed to allocate \"0\" bytes: zero bytes allocation is prohibited.\n");
			std::terminate();
		}
		else if (records_.find(p) == records_.end())
			records_.insert(p);
		else
		{
			fprintf(stderr, "Failed to allocate \"%u\" bytes: already allocated.\n", n);
			std::terminate();
		}
	}

	void do_deallocation(void* p, size_t n)
	{
		std::lock_guard lock(mutex_);
		const auto it = records_.find(p);
		if (it != records_.end())
			records_.erase(it);
		else
		{
			fprintf(stderr, "Failed to deallocate \"%u\" bytes: never was allocated or already deallocated.\n", n);
			std::terminate();
		}
	}

private:
	memory_debugger(const memory_debugger&) = delete;
	memory_debugger& operator=(const memory_debugger&) = delete;

	std::unordered_set<void*> records_;
	std::mutex mutex_;
};

template <typename UnderlyingAllocator>
class memory_debug_allocator : public UnderlyingAllocator
{
public:
	using underlying_allocator = UnderlyingAllocator;
	using value_type = typename UnderlyingAllocator::value_type;
	using T = value_type;

	constexpr memory_debug_allocator(memory_debugger& dbg) noexcept : dbg_(dbg) {}
	constexpr memory_debug_allocator(const memory_debug_allocator& other) noexcept = default;
	constexpr ~memory_debug_allocator() {}

	template <typename UnderlyingAllocator2>
	constexpr memory_debug_allocator(const memory_debug_allocator<UnderlyingAllocator2>& other) noexcept :
		UnderlyingAllocator(static_cast<typename memory_debug_allocator<UnderlyingAllocator2>::underlying_allocator&>(other))
	{
	};

	[[nodiscard]] constexpr T* allocate(size_t n) noexcept
	{
		T* ret = underlying_allocator::allocate(n);
		dbg_.do_allocation(ret, n);
		return ret;
	}

	constexpr void deallocate(T* p, size_t n) noexcept
	{
		underlying_allocator::deallocate(p, n);
		dbg_.do_deallocation(p, n);
	}

private:
	underlying_allocator alloc_;
	memory_debugger& dbg_;
};
} // namespace detail
} // namespace A3D

#endif // CONTAINER_DETAIL_MEMORY_DEBUG_ALLOCATOR_H
