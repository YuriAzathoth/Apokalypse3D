/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022 Yuriy Zinchenko

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

#include "RendererAllocator.h"

#ifndef BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#define BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif // BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

namespace A3D
{
RendererAllocator::~RendererAllocator()
{
}

void* RendererAllocator::realloc(void* _ptr,
								size_t _size,
								size_t _align,
								const char* _file,
								uint32_t _line)
{
	if (0 == _size)
	{
		if (NULL != _ptr)
		{
			if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
			{
				::free(_ptr);
				return NULL;
			}

#if BX_COMPILER_MSVC
			BX_UNUSED(_file, _line);
			_aligned_free(_ptr);
#else
			alignedFree(this, _ptr, _align, _file, _line);
#endif // BX_
		}

		return NULL;
	}
	else if (NULL == _ptr)
	{
		if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
			return ::malloc(_size);

#if BX_COMPILER_MSVC
		BX_UNUSED(_file, _line);
		return _aligned_malloc(_size, _align);
#else
		return alignedAlloc(this, _size, _align, _file, _line);
#endif // BX_
	}

	if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
		return ::realloc(_ptr, _size);

#if BX_COMPILER_MSVC
	BX_UNUSED(_file, _line);
	return _aligned_realloc(_ptr, _size, _align);
#else
	return alignedRealloc(this, _ptr, _size, _align, _file, _line);
#endif // BX_
}
} // namespace A3D
