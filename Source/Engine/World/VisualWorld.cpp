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

#include <cglm/cam.h>
#include <cglm/quat.h>
#include "Core/ILog.h"
#include "VisualWorld.h"

namespace A3D
{
VisualWorld::VisualWorld()
{
	CreateViewport(); // Create main viewport with handle index 0.
}

void VisualWorld::Prepare()
{
}

void VisualWorld::GetVisible(VisibleItemsArray& visibles, ViewportHandle camera)
{
}

ViewportHandle VisualWorld::CreateViewport()
{
	Camera camera{ GLM_MAT4_IDENTITY, GLM_MAT4_IDENTITY };
	ViewportRect size{};
	const ViewportIndex index = viewport_cameras_.emplace(camera);
	viewport_sizes_.insert(size);
	const ViewportHandle handle = { viewport_indices_.insert(index) };
	viewport_handles_.insert(handle);
	return handle;
}

void VisualWorld::RemoveViewport(ViewportHandle viewport)
{
	const ViewportIndex index = viewport_indices_[viewport.id];
	viewport_indices_.erase(viewport.id);

	const ViewportIndex rebound_index = viewport_cameras_.erase(index);
	viewport_sizes_.erase(index);
	viewport_handles_.erase(index);
	if (rebound_index != viewport_cameras_.INVALID_KEY)
		viewport_indices_[viewport_handles_[index].id] = index;
}

/*void CreateVisualWorld(VisualWorld& world)
{
	AddViewport(world);
}

void DestroyVisualWorld(VisualWorld& world)
{
	world.objects.ids.clear();
	world.objects.external_handles.clear();
	world.objects.global_transforms.clear();
	world.objects.materials.clear();
	world.objects.meshes.clear();

	world.viewports.ids.clear();
	world.viewports.external_handles.clear();
	world.viewports.cameras.clear();
	world.viewports.sizes.clear();

	world.visible.clear();
}

void PrepareVisualWorld(VisualWorld& world)
{
	auto mesh_it = world.objects.meshes.cbegin();
	auto material_it = world.objects.materials.cbegin();
	auto transform_it = world.objects.global_transforms.cbegin();
	const auto mesh_end = world.objects.meshes.cend();
	while (mesh_it != mesh_end)
	{
		world.visible.emplace_back(VisualRenderItem{*mesh_it, *material_it, *transform_it});
		++mesh_it;
		++material_it;
		++transform_it;
	}
}

void RenderVisualWorld(VisualWorld& world)
{
	BeginRendererFrame();

	SetCameraTransforms(world.viewports.cameras.data(), world.viewports.cameras.size());

	for (const VisualRenderItem& item : world.visible)
		DrawMeshGroup(item.mesh, item.material, item.transform);

	EndRendererFrame();

	world.visible.clear();
}

void RenderVisualWorld(VisualWorld& world, RendererThreadContext* contexts, uint8_t threads_count)
{
	BeginRendererFrame();
	BeginRendererThreadContextsFrame(contexts, threads_count);

	SetCameraTransforms(world.viewports.cameras.data(), world.viewports.cameras.size());

	const VisualRenderItem* item;
	for (uint16_t i = 0; i < world.visible.size(); ++i)
	{
		item = &world.visible[i];
		DrawMeshGroup(contexts[i % threads_count], item->mesh, item->material, item->transform);
	}

	EndRendererThreadContextsFrame(contexts, threads_count);
	EndRendererFrame();

	world.visible.clear();
}

ViewportHandle AddViewport(VisualWorld& world)
{
	const ViewportIndex index = world.viewports.cameras.emplace();
	world.viewports.sizes.emplace();
	const ViewportHandleType handle = world.viewports.ids.insert(index);
	world.viewports.external_handles.emplace(handle);
	return { handle };
}

void RemoveViewport(VisualWorld& world, ViewportHandle viewport)
{
	const ViewportIndex index = world.viewports.ids[viewport.id];
	const ViewportIndex rebound_index = world.viewports.cameras.erase(index);
	world.viewports.sizes.erase(index);
	if (rebound_index != world.viewports.cameras.INVALID_KEY)
		world.viewports.ids[world.viewports.external_handles[rebound_index]] = index;
}

void SetCameraPerspective(VisualWorld& world,
						  ViewportHandle viewport,
						  float fov,
						  float aspect,
						  float near,
						  float far)
{
	mat4& proj = world.viewports.cameras[world.viewports.ids[viewport.id]].proj;
	glm_perspective(fov, aspect, near, far, proj);
}

void SetCameraLookAt(VisualWorld& world,
					 ViewportHandle viewport,
					 vec3 position,
					 vec3 target)
{
	vec3 forward;
	glm_vec3_sub(target, position, forward);

	vec3 AXIS_X = { 1.0f, 0.0f, 0.0f };
	vec3 up;
	glm_vec3_copy(forward, up);
	glm_vec3_rotate(up, glm_rad(90.0f), AXIS_X);

	mat4& view = world.viewports.cameras[world.viewports.ids[viewport.id]].view;
	glm_lookat(position, target, up, view);
}

VisualHandle AddModel(VisualWorld& world, MeshGroup& mesh, Material& material, GlobalTransform& transform)
{
	const VisualIndex index = world.objects.meshes.insert(mesh);
	world.objects.materials.insert(material);
	world.objects.global_transforms.insert(transform);

	const VisualHandleType handle = world.objects.ids.insert(index);
	world.objects.external_handles.insert(handle);

	return { handle };
}

void RemoveModel(VisualWorld& world, VisualHandle handle)
{
	const VisualIndex index = world.objects.ids[handle.id];
	world.objects.ids.erase(handle.id);

	const VisualIndex rebound_index = world.objects.meshes.erase(index);
	world.objects.materials.erase(index);
	world.objects.global_transforms.erase(index);

	if (rebound_index != world.objects.meshes.INVALID_KEY)
		world.objects.ids[world.objects.external_handles[rebound_index]] = index;
	world.objects.external_handles.erase(index);
}*/
} // namespace A3D
