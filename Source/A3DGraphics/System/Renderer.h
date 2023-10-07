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

#ifndef SYSTEM_RENDERER_H
#define SYSTEM_RENDERER_H

#include <stdint.h>
#include "A3DGraphicsAPI.h"
#include "Common/RendererThreadContext.h"

namespace A3D
{
struct Camera;
struct GlobalTransform;
struct MeshGroup;
struct Technique;

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

A3DGRAPHICSAPI_EXPORT bool CreateRenderer(RendererGpu& gpu,
										   void* window,
										   void* display,
										   const RendererResolution& resolution,
										   RendererType type,
										   uint8_t anisotropy,
										   uint8_t msaa,
										   bool fullscreen,
										   bool vsync);

A3DGRAPHICSAPI_EXPORT void DestroyRenderer();

A3DGRAPHICSAPI_EXPORT void BeginRendererFrame();
A3DGRAPHICSAPI_EXPORT void EndRendererFrame();

A3DGRAPHICSAPI_EXPORT RendererThreadContext* CreateRendererThreadContexts(uint8_t count);
A3DGRAPHICSAPI_EXPORT void DestroyRendererThreadContexts(RendererThreadContext* contexts);
A3DGRAPHICSAPI_EXPORT void BeginRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);
A3DGRAPHICSAPI_EXPORT void EndRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count);

A3DGRAPHICSAPI_EXPORT void SetCameraTransforms(const Camera* cameras, uint16_t count);

A3DGRAPHICSAPI_EXPORT void DrawMeshGroup(const MeshGroup& mesh, const Technique& technique, const GlobalTransform& transform);

A3DGRAPHICSAPI_EXPORT void DrawMeshGroup(RendererThreadContext& context,
										 const MeshGroup& mesh,
										 const Technique& technique,
										 const GlobalTransform& transform);
} // namespace A3D

#endif // SYSTEM_RENDERER_H
