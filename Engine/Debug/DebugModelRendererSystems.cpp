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

#include <bgfx/bgfx.h>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include "DebugModelRendererSystems.h"
#include "Graphics/GeometryComponents.h"
#include "Graphics/GraphicsResourceComponents.h"
#include "Graphics/RendererComponents.h"
#include "IO/Log.h"
#include "Scene/SceneComponents.h"

using namespace A3D::Components::Geometry;
using namespace A3D::Components::Renderer;
using namespace A3D::Components::Resource::Graphics;
using namespace A3D::Components::Scene;

namespace A3D
{
static void draw_geometry(flecs::entity e, const Node& node, const GpuProgram& program)
{
	LogTrace("Drawing model from thread %d...", std::this_thread::get_id());

	flecs::world w = e.world();
	const Renderer* renderer = w.get<Renderer>();

	unsigned thread_id;
	if (renderer->threads.size() > 1)
	{
		const auto thread_id_tmp = std::this_thread::get_id();
		thread_id = *reinterpret_cast<const unsigned*>(&thread_id_tmp) - 2;
		ecs_assert(thread_id <= renderer->threads.size(), -1,
				   "Render called from thread \"%d\" that not registered.");
	}
	else
		thread_id = 0;

	bgfx::Encoder* queue = renderer->threads[thread_id].queue;
	if (queue)
		e.children([&node, &program, queue, &w](flecs::entity c)
		{
			const MeshGroup* mesh = c.get<MeshGroup>();
			if (mesh)
			{
				LogTrace("Drawing static mesh...");
				ecs_assert_var(bgfx::isValid(mesh->vbo), -1, "Failed to render node: invalid vertex buffer handle.");
				ecs_assert_var(bgfx::isValid(mesh->ebo), -1, "Failed to render node: invalid elements buffer handle.");
				ecs_assert_var(bgfx::isValid(program.handle), -1, "Failed to render node: invalid GPU program handle.");
				queue->setTransform(glm::value_ptr(node.model));
				queue->setVertexBuffer(0, mesh->vbo);
				queue->setIndexBuffer(mesh->ebo);
				queue->setState(BGFX_STATE_DEFAULT);
				queue->submit(0, program.handle);
			}
		});
}

DebugModelRendererSystems::DebugModelRendererSystems(flecs::world& world)
{
	world.import<GraphicsResourceComponents>();
	world.import<GeometryComponents>();
	world.import<RendererComponents>();
	world.import<SceneComponents>();

	flecs::_::cpp_type<DebugModelRendererSystems>::id_explicit(world, 0, false);
	world.module<DebugModelRendererSystems>("A3D::Systems::DebugRenderer");

	drawSingleThreaded_ = world.system<const Node, const GpuProgram>("DrawSingleThread")
		.term<const Model>()
		.term<const MultiThreaded>().not_().singleton()
		.kind(flecs::PreStore)
		.each(draw_geometry);

	drawMultiThreaded_ = world.system<const Node, const GpuProgram>("DrawMultiThread")
		.term<const Model>()
		.term<const MultiThreaded>().singleton()
		.kind(flecs::PreStore)
		.multi_threaded()
		.each(draw_geometry);
}
} // namespace A3D
