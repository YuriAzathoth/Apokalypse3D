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

#ifndef WORLD_VISUAL_WORLD_H
#define WORLD_VISUAL_WORLD_H

#include <cglm/types.h>
#include <stdint.h>
#include "A3DGraphicsAPI.h"
#include "Common/Camera.h"
#include "Common/Geometry.h"
#include "Common/GpuProgram.h"
#include "Common/Model.h"
#include "Common/Viewport.h"
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/vector.h"

namespace A3D
{
using ViewportIndex = uint16_t;
using ViewportHandleType = uint16_t;
struct ViewportHandle
{
	ViewportHandleType id;
};

using VisualIndex = uint16_t;
using VisualHandleType = uint16_t;
struct VisualHandle
{
	VisualHandleType id;
};

inline static const ViewportHandle MAIN_VIEWPORT = {0};

struct VisualRenderItem
{
	MeshGroup mesh;
	GpuProgram program;
	GlobalTransform transform;
};

struct VisualViewportStore
{
	dense_map<ViewportIndex, Camera> cameras;
	dense_map<ViewportIndex, ViewportRect> sizes;
	dense_map<ViewportIndex, ViewportHandleType> external_handles;
	sparse_map<ViewportHandleType, ViewportIndex> ids;
};

struct VisualObjectsStore
{
	dense_map<VisualIndex, MeshGroup> meshes;
	dense_map<VisualIndex, GpuProgram> gpu_programs;
	dense_map<VisualIndex, GlobalTransform> global_transforms;
	dense_map<VisualIndex, VisualHandleType> external_handles;
	sparse_map<VisualHandleType, VisualIndex> ids;
};

struct VisualWorld
{
	VisualObjectsStore objects;
	VisualViewportStore viewports;

	vector<uint32_t, VisualRenderItem> visible;
};

A3DGRAPHICSAPI_EXPORT void CreateVisualWorld(VisualWorld& world);
A3DGRAPHICSAPI_EXPORT void DestroyVisualWorld(VisualWorld& world);

A3DGRAPHICSAPI_EXPORT void PrepareVisualWorld(VisualWorld& world);
A3DGRAPHICSAPI_EXPORT void RenderVisualWorld(VisualWorld& world);

A3DGRAPHICSAPI_EXPORT ViewportHandle AddViewport(VisualWorld& world);
A3DGRAPHICSAPI_EXPORT void RemoveViewport(VisualWorld& world, ViewportHandle viewport);
A3DGRAPHICSAPI_EXPORT void SetCameraPerspective(VisualWorld& world,
												ViewportHandle viewport,
												float fov,
												float aspect,
												float near,
												float far);
A3DGRAPHICSAPI_EXPORT void SetCameraLookAt(VisualWorld& world,
										   ViewportHandle viewport,
										   vec3 position,
										   vec3 target);

A3DGRAPHICSAPI_EXPORT VisualHandle AddModel(VisualWorld& world,
											MeshGroup& mesh,
											GpuProgram& program,
											GlobalTransform& transform);
A3DGRAPHICSAPI_EXPORT void RemoveModel(VisualWorld& world, VisualHandle handle);

inline static GlobalTransform& GetVisualObjectTransform(VisualWorld& world, VisualHandle handle)
{
	return world.objects.global_transforms[world.objects.ids[handle.id]];
}
} // namespace A3D

#endif // WORLD_VISUAL_WORLD_H
