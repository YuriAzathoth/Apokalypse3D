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

#include <stdint.h>
#include <stdlib.h>
#include <unordered_map>

struct DebugAllocatorBase
{
	static std::unordered_map<void*, int> s_allocs;
};

std::unordered_map<void*, int> DebugAllocatorBase::s_allocs;

template <typename T>
struct DebugAllocator : DebugAllocatorBase
{
	using value_type = T;
	using size_type = uint32_t;
	using difference_type = int32_t;

	T* allocate(size_t size)
	{
		T* ret = (T*)malloc(size * sizeof(T));
		s_allocs.emplace(ret, 1);
		return ret;
	}

	void deallocate(T* ptr, size_t)
	{
		auto it = s_allocs.find(ptr);
		REQUIRE(it != s_allocs.end());
		REQUIRE(it->second == 1);
		s_allocs.erase(it);
		free(ptr);
	}
};

void CheckMemoryLeaks()
{
	REQUIRE(DebugAllocatorBase::s_allocs.empty());
}
