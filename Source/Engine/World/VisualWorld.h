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

#ifndef WORLD_VISUAL_WORLD_H
#define WORLD_VISUAL_WORLD_H

#include <cglm/types.h>
#include <stdint.h>
#include "EngineAPI.h"
#include "Common/Camera.h"
#include "Common/Geometry.h"
#include "Common/Material.h"
#include "Common/Model.h"
#include "Common/Viewport.h"
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/vector.h"

namespace A3D
{
using RenderableIndex = uint32_t;
using RenderableHandleType = uint32_t;

struct RenderableHandle
{
	RenderableHandleType id;
};

using ViewportIndex = uint16_t;
using ViewportHandleType = uint16_t;

struct ViewportHandle
{
	ViewportHandleType id;
};

inline static constexpr ViewportHandle MAIN_VIEWPORT = {0};

struct VisibleRenderItem
{
	MeshGroup mesh;
	Material material;
	GlobalTransform transform;
};

using VisibleItemsArray = vector<uint32_t, VisibleRenderItem>;

class ENGINEAPI_EXPORT VisualWorld
{
public:
	VisualWorld();
	~VisualWorld() {}

	void Prepare();
	void GetVisible(VisibleItemsArray& visibles, ViewportHandle camera);

	ViewportHandle CreateViewport();

	void RemoveViewport(ViewportHandle viewport);

	Camera& GetViewportCamera(ViewportHandle viewport)
	{
		return viewport_cameras_[viewport_indices_[viewport.id]];
	}

	ViewportRect& GetViewportSize(ViewportHandle viewport)
	{
		return viewport_sizes_[viewport_indices_[viewport.id]];
	}

	const Camera& GetViewportCamera(ViewportHandle viewport) const
	{
		return viewport_cameras_[viewport_indices_[viewport.id]];
	}

	const ViewportRect& GetViewportSize(ViewportHandle viewport) const
	{
		return viewport_sizes_[viewport_indices_[viewport.id]];
	}

private:
	sparse_map<RenderableHandleType, RenderableIndex> renderable_indices_;
	dense_map<RenderableIndex, MeshGroup> renderable_meshes_;
	dense_map<RenderableIndex, Material> renderable_materials_;
	dense_map<RenderableIndex, GlobalTransform> global_transforms_;
	dense_map<RenderableIndex, RenderableHandle> renderable_handles_;

	sparse_map<ViewportHandleType, ViewportIndex> viewport_indices_;
	dense_map<ViewportIndex, Camera> viewport_cameras_;
	dense_map<ViewportIndex, ViewportRect> viewport_sizes_;
	dense_map<ViewportIndex, ViewportHandle> viewport_handles_;
};

/*using ViewportIndex = uint16_t;
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
	Material material;
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
	dense_map<VisualIndex, Material> materials;
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

ENGINEAPI_EXPORT void CreateVisualWorld(VisualWorld& world);
ENGINEAPI_EXPORT void DestroyVisualWorld(VisualWorld& world);

ENGINEAPI_EXPORT void PrepareVisualWorld(VisualWorld& world);
ENGINEAPI_EXPORT void RenderVisualWorld(VisualWorld& world);
ENGINEAPI_EXPORT void RenderVisualWorld(VisualWorld& world, RendererThreadContext* contexts, uint8_t threads_count);

ENGINEAPI_EXPORT ViewportHandle AddViewport(VisualWorld& world);
ENGINEAPI_EXPORT void RemoveViewport(VisualWorld& world, ViewportHandle viewport);

ENGINEAPI_EXPORT void SetCameraPerspective(VisualWorld& world,
												ViewportHandle viewport,
												float fov,
												float aspect,
												float near,
												float far);

ENGINEAPI_EXPORT void SetCameraLookAt(VisualWorld& world, ViewportHandle viewport, vec3 position, vec3 target);

ENGINEAPI_EXPORT VisualHandle AddModel(VisualWorld& world, MeshGroup& mesh, Material& material, GlobalTransform& transform);
ENGINEAPI_EXPORT void RemoveModel(VisualWorld& world, VisualHandle handle);

inline static GlobalTransform& GetVisualObjectTransform(VisualWorld& world, VisualHandle handle)
{
	return world.objects.global_transforms[world.objects.ids[handle.id]];
}*/
} // namespace A3D

#endif // WORLD_VISUAL_WORLD_H
