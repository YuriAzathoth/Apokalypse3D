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

#ifndef CORE_DEFAULT_ALLOCATOR_H
#define CORE_DEFAULT_ALLOCATOR_H

#include "IAllocator.h"

namespace A3D
{
struct DefaultAllocator : public IAllocator
{
	void* Allocate(size_t size) override;

	void Deallocate(void* ptr) override;
};
} // namespace A3D

#endif // CORE_DEFAULT_ALLOCATOR_H
