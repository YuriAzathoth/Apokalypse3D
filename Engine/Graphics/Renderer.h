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

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <stdint.h>
#include "Apokalypse3DAPI.h"

namespace bgfx { struct Encoder; }

namespace A3D
{
struct RendererGpu
{
	uint16_t device;
	uint16_t vendor;
};

struct RendererResolution
{
	uint16_t width;
	uint16_t height;
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

struct RendererThreadContext
{
	bgfx::Encoder* queue;
};

APOKALYPSE3DAPI_EXPORT bool CreateRenderer(RendererGpu& gpu,
										   void* window,
										   void* display,
										   const RendererResolution& resolution,
										   RendererType type,
										   uint8_t anisotropy,
										   uint8_t msaa,
										   bool fullscreen,
										   bool vsync);

APOKALYPSE3DAPI_EXPORT void DestroyRenderer();

APOKALYPSE3DAPI_EXPORT void BeginRendererFrame(const RendererResolution& resolution);
APOKALYPSE3DAPI_EXPORT void EndRendererFrame();

APOKALYPSE3DAPI_EXPORT struct RendererThreadContext* CreateRendererThreadContexts(uint8_t count);
APOKALYPSE3DAPI_EXPORT void DestroyRendererThreadContexts(RendererThreadContext* contexts);
APOKALYPSE3DAPI_EXPORT void BeginRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);
APOKALYPSE3DAPI_EXPORT void EndRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);
} // namespace A3D

#endif // GRAPHICS_RENDERER_H
