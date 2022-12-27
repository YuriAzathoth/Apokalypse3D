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

#ifndef RENDERERCOMPONENTS_H
#define RENDERERCOMPONENTS_H

#include <bx/file.h>
#include <flecs.h>
#include <vector>
#include "Apokalypse3DAPI.h"

namespace bgfx { struct Encoder; }

namespace A3D
{
namespace Components
{
namespace Renderer
{
struct Thread
{
	mutable bgfx::Encoder* queue;
};

struct Device
{
	uint16_t device;
	uint16_t vendor;
};

struct Renderer
{
	std::vector<Thread> threads;
	bx::DefaultAllocator alloc;
};

struct Startup {};
} // namespace Renderer
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT RendererComponents
{
	RendererComponents(flecs::world& world);

	flecs::entity device_;
	flecs::entity renderer_;
	flecs::entity startup_;
};
} // namespace A3D

#endif // RENDERERCOMPONENTS_H
