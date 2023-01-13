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
#include "BruteRendererSystems.h"
#include "Graphics/GeometryComponents.h"
#include "Graphics/GpuProgramComponents.h"
#include "Graphics/GraphicsComponents.h"
#include "Graphics/MeshComponents.h"
#include "Graphics/RendererComponents.h"
#include "IO/Log.h"
#include "Scene/SceneComponents.h"

using namespace A3D::Components::Geometry;
using namespace A3D::Components::GpuProgram;
using namespace A3D::Components::Graphics;
using namespace A3D::Components::Mesh;
using namespace A3D::Components::Renderer;
using namespace A3D::Components::Scene;

static void draw_geometry_st(flecs::entity e, const WorldTransform& wt, const Program& program)
{
	e.children([&program, &wt](flecs::entity c)
	{
		const MeshGroup* mesh = c.get<MeshGroup>();
		if (mesh)
		{
			Assert(bgfx::isValid(mesh->vbo), "Failed to render node: invalid vertex buffer handle.");
			Assert(bgfx::isValid(mesh->ebo), "Failed to render node: invalid elements buffer handle.");
			Assert(bgfx::isValid(program.handle), "Failed to render node: invalid GPU program handle.");
			bgfx::setTransform(glm::value_ptr(wt.global));
			bgfx::setVertexBuffer(0, mesh->vbo);
			bgfx::setIndexBuffer(mesh->ebo);
			bgfx::setState(BGFX_STATE_DEFAULT);
			bgfx::submit(0, program.handle);
		}
	});
}

static void draw_geometry_mt(flecs::entity e, const WorldTransform& wt, const Program& program, const RendererThreads& threads)
{
	const unsigned thread_id = e.world().get_stage_id();
	Assert(thread_id <= threads.count, "Render called from thread \"%d\" that not registered.");
	bgfx::Encoder* queue = threads.queues[thread_id];
	if (queue)
		e.children([&program, queue, &wt](flecs::entity c)
		{
			const MeshGroup* mesh = c.get<MeshGroup>();
			if (mesh)
			{
				Assert(bgfx::isValid(mesh->vbo), "Failed to render node: invalid vertex buffer handle.");
				Assert(bgfx::isValid(mesh->ebo), "Failed to render node: invalid elements buffer handle.");
				Assert(bgfx::isValid(program.handle), "Failed to render node: invalid GPU program handle.");
				queue->setTransform(glm::value_ptr(wt.global));
				queue->setVertexBuffer(0, mesh->vbo);
				queue->setIndexBuffer(mesh->ebo);
				queue->setState(BGFX_STATE_DEFAULT);
				queue->submit(0, program.handle);
			}
		});
}

A3D::BruteRendererSystems::BruteRendererSystems(flecs::world& world)
{
	world.module<BruteRendererSystems>("A3D::Systems::BruteRenderer");
	world.import<GpuProgramComponents>();
	world.import<GeometryComponents>();
	world.import<GraphicsComponents>();
	world.import<MeshComponents>();
	world.import<RendererComponents>();
	world.import<SceneComponents>();

	drawSingleThreaded_ = world.system<const WorldTransform, const Program>("DrawSingleThread")
		.with<MultiThreaded>().singleton().not_()
		.with<Renderer>().singleton()
		.with<Model>()
		.kind(flecs::PreStore)
		.each(draw_geometry_st);

	drawMultiThreaded_ = world.system<const WorldTransform, const Program, const RendererThreads>("DrawMultiThread")
		.arg(3).singleton()
		.with<MultiThreaded>().singleton()
		.with<Renderer>().singleton()
		.with<Model>()
		.kind(flecs::PreStore)
		.multi_threaded()
		.each(draw_geometry_mt);
}
