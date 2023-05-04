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

#include "IO/Log.h"
#include "System/Renderer.h"
#include "VisualWorld.h"

namespace A3D
{
void CreateVisualWorld(VisualWorld& world)
{
	world.draw_queue.state.store(DrawQueueState::RECEIVING);
}

void DestroyVisualWorld(VisualWorld& world)
{
}

void PrepareVisualWorld(VisualWorld& world)
{
	auto mesh_it = world.visual_data.cbegin();
	auto program_it = world.gpu_programs.cbegin();
	auto transform_it = world.global_transforms.cbegin();
	const auto mesh_end = world.visual_data.cend();
	while (mesh_it != mesh_end)
	{
		PushDrawQueue(world.draw_queue, *mesh_it, *program_it, *transform_it);
		++mesh_it;
		++program_it;
		++transform_it;
	}
}

void RenderVisualWorld(VisualWorld& world)
{
	BeginRendererFrame();
	BeginDrawQueue(world.draw_queue);

	SetCameraTransform(world.main_camera);

	auto range = GetDrawQueueItems(world.draw_queue);
	for (; range.first != range.second; ++range.first)
	{
		DrawMeshGroup(range.first->mesh, range.first->program, range.first->transform);
	}

	EndDrawQueue(world.draw_queue);
	EndRendererFrame();
}

VisualHandle AddModel(VisualWorld& world, MeshGroup& mesh, GpuProgram& program, GlobalTransform& transform)
{
	const VisualIndex index = world.visual_data.insert(mesh);
	world.gpu_programs.insert(program);
	world.global_transforms.insert(transform);

	const VisualHandleType handle = world.ids.insert(index);
	world.external_handles.insert(handle);

	return { handle };
}

void RemoveModel(VisualWorld& world, VisualHandle handle)
{
	const VisualIndex index = world.ids[handle.id];
	world.ids.erase(handle.id);

	const VisualIndex rebound_index = world.visual_data.erase(index);
	world.gpu_programs.erase(index);
	world.global_transforms.erase(index);

	if (rebound_index != decltype(world.visual_data)::INVALID_KEY)
	{
		world.ids[rebound_index] = index;
	}
}
} // namespace A3D
