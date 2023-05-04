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

#include <stdint.h>
#include <vector>
#include "A3DGraphicsAPI.h"
#include "Common/Camera.h"
#include "Common/Geometry.h"
#include "Common/GpuProgram.h"
#include "Common/Model.h"
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Graphics/DrawQueue.h"

namespace A3D
{
using VisualIndex = uint16_t;
using VisualHandleType = uint16_t;
struct VisualHandle
{
	VisualHandleType id;
};

struct VisualWorld
{
	dense_map<VisualIndex, MeshGroup> visual_data;
	dense_map<VisualIndex, GpuProgram> gpu_programs;
	dense_map<VisualIndex, GlobalTransform> global_transforms;
	dense_map<VisualIndex, VisualHandleType> external_handles;
	sparse_map<VisualHandleType, VisualIndex> ids;

	DrawQueue draw_queue;

	Camera main_camera;
};

A3DGRAPHICSAPI_EXPORT void CreateVisualWorld(VisualWorld& world);
A3DGRAPHICSAPI_EXPORT void DestroyVisualWorld(VisualWorld& world);

A3DGRAPHICSAPI_EXPORT void PrepareVisualWorld(VisualWorld& world);
A3DGRAPHICSAPI_EXPORT void RenderVisualWorld(VisualWorld& world);

A3DGRAPHICSAPI_EXPORT VisualHandle AddModel(VisualWorld& world,
											MeshGroup& mesh,
											GpuProgram& program,
											GlobalTransform& transform);
A3DGRAPHICSAPI_EXPORT void RemoveModel(VisualWorld& world, VisualHandle handle);

inline static GlobalTransform& GetVisualWorldTransform(VisualWorld& world, VisualHandle handle)
{
	return world.global_transforms[world.ids[handle.id]];
}
} // namespace A3D

#endif // WORLD_VISUAL_WORLD_H
