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

#include <flecs.h>
#include <glm/gtx/quaternion.hpp>
#include <glm/trigonometric.hpp>
#include "Core/Coordinator.h"
#include "Core/MainThreadSystem.h"
#include "Graphics/Graphics.h"
#include "Graphics/GraphicsComponents.h"
#include "Scene/Scene.h"
#include "Scene/SceneComponents.h"

struct Vertex
{
	float x;
	float y;
	float z;
	float u;
	float v;
};

static constexpr Vertex VERTICES[] = {
	{-1.0f, 1.0f, 1.0f, 0.0f, 0.0f},	// LUF
	{1.0f, 1.0f, 1.0f, 1.0f, 0.0f},		// RUF
	{-1.0f, -1.0f, 1.0f, 0.0f, 1.0f},	// LDF
	{1.0f, -1.0f, 1.0f, 1.0f, 1.0f},	// RDF
	{-1.0f, 1.0f, -1.0f, 1.0f, 0.0f},	// LUB
	{1.0f, 1.0f, -1.0f, 0.0f, 0.0f},	// RUB
	{-1.0f, -1.0f, -1.0f, 1.0f, 1.0f},	// LDB
	{1.0f, -1.0f, -1.0f, 0.0f, 1.0f},	// RDB
	{-1.0f, 1.0f, -1.0f, 0.0f, 1.0f},	// LUB
	{1.0f, 1.0f, -1.0f, 1.0f, 1.0f},	// RUB
	{-1.0f, -1.0f, -1.0f, 0.0f, 0.0f},	// LDB
	{1.0f, -1.0f, -1.0f, 1.0f, 0.0f},	// RDB
};

static constexpr uint16_t INDICES[] = {
	0, 1, 2, 1, 3, 2, // Front
	4, 6, 5, 5, 6, 7, // Back
	0, 2, 4, 4, 2, 6, // Left
	1, 5, 3, 5, 7, 3, // Right
	0, 8, 1, 8, 9, 1, // Top
	2, 3, 10, 10, 3, 11, // Bottom
};

void CreateBoxes(flecs::world& world,
				 flecs::entity parent,
				 const Model& model,
				 const MaterialBasic& material,
				 float scale,
				 unsigned levels)
{
	glm::mat4 identity(1.0f);
	const glm::vec3 dist(5.0f, 0.0f, 0.0f);
	const glm::vec3 size(scale, scale, scale);
	glm::mat4 tr = glm::scale(glm::translate(identity, dist), size);
	flecs::entity box = world.entity("Model1")
							  .child_of(parent)
							  .add<Node>()
							  .set<Translation>({tr})
							  .set<Model>(model)
							  .set<MaterialBasic>(material)
							  .set<Rotate>({{glm::radians(90.0f), glm::radians(90.0f), 0.0f}});

	glm::quat rot = glm::angleAxis(glm::radians(90.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::scale(glm::translate(identity, rot * dist), size);
	box = world.entity("Model2")
				.child_of(parent)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>(model)
				.set<MaterialBasic>(material)
				.set<Rotate>({{glm::radians(90.0f), 0.0f, glm::radians(90.0f)}});

	rot = glm::angleAxis(glm::radians(180.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::scale(glm::translate(identity, rot * dist), size);
	box = world.entity("Model3")
				.child_of(parent)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>(model)
				.set<MaterialBasic>(material)
				.set<Rotate>({{glm::radians(-90.0f), glm::radians(-90.0f), 0.0f}});

	rot = glm::angleAxis(glm::radians(270.0f), glm::vec3{0.0f, 0.0f, 1.0f});
	tr = glm::scale(glm::translate(identity, rot * dist), size);
	box = world.entity("Model4")
				.child_of(parent)
				.add<Node>()
				.set<Translation>({tr})
				.set<Model>(model)
				.set<MaterialBasic>(material)
				.set<Rotate>({{glm::radians(-90.0f), 0.0f, glm::radians(-90.0f)}});

	if (levels)
		parent.children([&world, &model, material, levels](flecs::entity child)
		{
			CreateBoxes(world, child, model, material, 0.75f, levels - 1);
		});
}

int main()
{
	flecs::world world;
	world.set_threads(4);
	RegisterGraphics(world);
	RegisterMainThreadSystem(world);
	RegisterScene(world);

	world.set<WindowCreate>({"02_Scene"});
	Run(world);

	/*Coordinator::InitInfo initInfo;
	initInfo.graphics.title = "Hello World";
	initInfo.graphics.render = Graphics::RenderType::OpenGL;
	initInfo.graphics.msaa = Graphics::MSAA::NONE;
	initInfo.graphics.display = 0;
	initInfo.graphics.width = 1024;
	initInfo.graphics.height = 768;
	initInfo.graphics.fullscreen = false;
	initInfo.graphics.highDpi = true;
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
	bgfx::TextureHandle texture = graphics.LoadTexture("SampleCrate.ktx");
	const Model model = {vbo, ebo};
	const MaterialBasic material = { program, texture };

	if (!(bgfx::isValid(vbo) &&
		  bgfx::isValid(ebo) &&
		  bgfx::isValid(vertex) &&
		  bgfx::isValid(fragment) &&
		  bgfx::isValid(program) &&
		  bgfx::isValid(texture)))
		return 1;

	flecs::world& world = coordinator.GetWorld();
	flecs::entity scene = coordinator.GetScene().GetRoot();

	flecs::entity camera = world.entity("Camera")
							   .child_of(scene)
							   .add<Node>()
							   .add<Camera>()
							   .set<Perspective>({glm::radians(45.0f), 0.0f, 0.1f, 100.0f})
							   .set<Ray>({{0.0f, 0.0f, 20.0f}, {0.0f, 0.0f, 0.0f}})
							   .add<WindowAspect>();

	flecs::entity origin = world.entity("Origin")
								.child_of(scene)
								.add<Node>()
								.add<Translation>()
								.set<Model>(model)
								.set<MaterialBasic>(material)
								.set<Rotate>({{0.0f, 0.0f, glm::radians(45.0f)}});

	CreateBoxes(world, origin, model, material, 0.75f, 4);*/

	//coordinator.Run();

	/*bgfx::destroy(vertex);
	bgfx::destroy(fragment);
	bgfx::destroy(program);
	bgfx::destroy(vbo);
	bgfx::destroy(ebo);
	bgfx::destroy(texture);*/

	return 0;
}
