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

#ifndef GRAPHICSCOMPONENTS_H
#define GRAPHICSCOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Graphics
{
enum class MsaaLevel : unsigned
{
	NONE,
	X2,
	X4,
	X8,
	X16
};

enum class RendererType : unsigned
{
	Auto,
	OpenGL,
	Vulkan,
#if defined(__WIN32__)
	Direct3D9,
	Direct3D11,
	Direct3D12,
#elif defined(OSX)
	Metal,
#endif // defined
	None
};

enum class WindowMode : unsigned
{
	Windowed,
	Fullscreen,
	Borderless
};

struct MultiThreaded {};
struct Resizeable {};

struct Resolution
{
	uint16_t width;
	uint16_t height;
};

struct VSync {};
} // namespace Graphics
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT GraphicsComponents
{
	GraphicsComponents(flecs::world& world);

	flecs::entity multiThreaded_;
	flecs::entity msaaLevel_;
	flecs::entity rendererType_;
	flecs::entity resizeable_;
	flecs::entity resolution_;
	flecs::entity vsync_;
	flecs::entity windowMode_;
};
} // namespace A3D

#endif // GRAPHICSCOMPONENTS_H
