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

#include <glm/gtx/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include "Core/Coordinator.h"
#include "Scene/SceneComponents.h"

struct PosColorVertex
{
	float x;
	float y;
	float z;
	uint32_t abgr;
};

static constexpr PosColorVertex VERTICES[] = {
	{-1.0f, 1.0f, 1.0f, 0xff000000},
	{1.0f, 1.0f, 1.0f, 0xff0000ff},
	{-1.0f, -1.0f, 1.0f, 0xff00ff00},
	{1.0f, -1.0f, 1.0f, 0xff00ffff},
	{-1.0f, 1.0f, -1.0f, 0xffff0000},
	{1.0f, 1.0f, -1.0f, 0xffff00ff},
	{-1.0f, -1.0f, -1.0f, 0xffffff00},
	{1.0f, -1.0f, -1.0f, 0xffffffff},
};

static constexpr uint16_t INDICES[] = {
	0, 1, 2, 1, 3, 2, 4, 6, 5, 5, 6, 7, 0, 2, 4, 4, 2, 6, 1, 5, 3, 5, 7, 3, 0, 4, 1, 4, 5, 1, 2, 3, 6, 6, 3, 7,
};

int main()
{
	Coordinator::InitInfo initInfo;
	initInfo.graphics.title = "Hello World";
	initInfo.graphics.width = 800;
	initInfo.graphics.height = 600;
	initInfo.graphics.vsync = true;

	bool initialized;
	Coordinator coordinator(initInfo, initialized);
	if (!initialized)
		return 1;

	Graphics& graphics = coordinator.GetGraphics();
	bgfx::VertexBufferHandle vbo = graphics.CreateVertexBuffer(VERTICES, sizeof(VERTICES));
	bgfx::IndexBufferHandle ebo = graphics.CreateIndexBuffer(INDICES, sizeof(INDICES));
	bgfx::ShaderHandle vertex = graphics.LoadShader("01_hello_world.vert.shader");
	bgfx::ShaderHandle fragment = graphics.LoadShader("01_hello_world.frag.shader");
	bgfx::ProgramHandle program = graphics.CreateProgram(vertex, fragment);

	flecs::world& world = coordinator.GetWorld();
	flecs::entity& scene = coordinator.GetScene();

	flecs::entity camera = world.entity("Camera")
							   .child_of(scene)
							   .add<Node>()
							   .add<Camera>()
							   .set<Ray>({{0.0f, 0.0f, -20.0f}, {0.0f, 0.0f, 0.0f}})
							   .set<Perspective>({glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f});

	flecs::entity origin = world.entity("Origin").child_of(scene).add<Node>().add<Translation>().set<Rotate>(
		{{0.0f, 0.0f, glm::radians(45.0f)}});

	glm::mat4 identity(1.0f);
	glm::vec3 dist(5.0f, 0.0f, 0.0f);
	glm::mat4 tr = glm::translate(identity, dist);
	flecs::entity model = world.entity("Model1")
							  .child_of(origin)
							  .add<Node>()
							  .set<Translation>({tr})
							  .set<Model>({vbo, ebo, program})
							  .set<Rotate>({{glm::radians(90.0f), glm::radians(90.0f), 0.0f}});

	glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::translate(identity, rot * dist);
	model = world.entity("Model2")
				.child_of(origin)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>({vbo, ebo, program})
				.set<Rotate>({{glm::radians(90.0f), 0.0f, glm::radians(90.0f)}});

	rot = glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::translate(identity, rot * dist);
	model = world.entity("Model3")
				.child_of(origin)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>({vbo, ebo, program})
				.set<Rotate>({{glm::radians(-90.0f), glm::radians(-90.0f), 0.0f}});

	rot = glm::angleAxis(glm::radians(270.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::translate(identity, rot * dist);
	model = world.entity("Model4")
				.child_of(origin)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>({vbo, ebo, program})
				.set<Rotate>({{glm::radians(-90.0f), 0.0f, glm::radians(-90.0f)}});

	dist = glm::vec3(2.0f, 0.0f, 0.0f);
	tr = glm::scale(glm::translate(identity, dist), {0.2f, 0.2f, 0.2f});
	model = world.entity("Model5").child_of(model).add<Node>().set<Translation>({tr}).set<Model>({vbo, ebo, program});

	return coordinator.Run();
}