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

#ifndef GRAPHICS_RENDERER_ALLOCATOR_H
#define GRAPHICS_RENDERER_ALLOCATOR_H

#include <bx/allocator.h>

namespace A3D
{
class IAllocator;

class RendererAllocator : public bx::AllocatorI
{
public:
	explicit RendererAllocator(IAllocator* alloc) : alloc_(alloc) {}

	void* realloc(void* _ptr,
				  size_t _size,
				  size_t _align,
				  const char* _file,
				  uint32_t _line) override;
protected:
	IAllocator* alloc_;
};
} // namespace A3D

#endif // GRAPHICS_RENDERER_ALLOCATOR_H
