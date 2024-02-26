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

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <stdint.h>
#include "EngineAPI.h"
#include "Common/RendererThreadContext.h"

namespace A3D
{
struct Camera;
struct GlobalTransform;
struct Material;
struct MeshGroup;

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

ENGINEAPI_EXPORT bool CreateRenderer(RendererGpu& gpu,
										   void* window,
										   void* display,
										   const RendererResolution& resolution,
										   RendererType type,
										   uint8_t anisotropy,
										   uint8_t msaa,
										   bool fullscreen,
										   bool vsync);

ENGINEAPI_EXPORT void DestroyRenderer();

ENGINEAPI_EXPORT void BeginRendererFrame();
ENGINEAPI_EXPORT void EndRendererFrame();

ENGINEAPI_EXPORT RendererThreadContext* CreateRendererThreadContexts(uint8_t count);
ENGINEAPI_EXPORT void DestroyRendererThreadContexts(RendererThreadContext* contexts);
ENGINEAPI_EXPORT void BeginRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);
ENGINEAPI_EXPORT void EndRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);

ENGINEAPI_EXPORT void SetCameraTransforms(const Camera* cameras, uint16_t count);

ENGINEAPI_EXPORT void DrawMeshGroup(const MeshGroup& mesh, const Material& material, const GlobalTransform& transform);

ENGINEAPI_EXPORT void DrawMeshGroup(RendererThreadContext& context,
										 const MeshGroup& mesh,
										 const Material& material,
										 const GlobalTransform& transform);
} // namespace A3D

#endif // GRAPHICS_RENDERER_H
