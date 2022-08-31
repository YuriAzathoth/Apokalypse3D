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
#include <glm/gtx/quaternion.hpp>
#include <glm/glm.hpp>
#include <thread>
#include "Core/Engine.h"
#include "Debug/DebugModelRendererSystems.h"
#include "Graphics/CameraComponents.h"
#include "Graphics/CameraSystems.h"
#include "Graphics/GraphicsResourceComponents.h"
#include "Graphics/GraphicsResourceSystems.h"
#include "Graphics/RendererComponents.h"
#include "Graphics/RendererSystems.h"
#include "Graphics/WindowComponents.h"
#include "Graphics/WindowSystems.h"
#include "Input/EventComponents.h"
#include "Input/EventSystems.h"
#include "Scene/SceneComponents.h"
#include "Scene/SceneSystems.h"

using namespace A3D;
using namespace A3D::Components;
using namespace ::Components;

void CreateBoxes(flecs::entity parent,
				 const char* model,
				 const char* vertex,
				 const char* fragment,
				 float scale,
				 unsigned children,
				 unsigned levels,
				 glm::mat4 transform = glm::mat4{1.0f})
{
	flecs::entity cube = parent.world().entity().child_of(parent)
		.add<Scene::Node>()
		.set<Scene::Translation>({transform})
		.set<Scene::Rotate>({{0.0f, 0.0f, 1.0f}})
		.set<Resource::Graphics::GetModelFile>({model})
		.set<Resource::Graphics::GetGpuProgram>({vertex, fragment});

	if (levels > 1)
	{
		const glm::vec3 pos{5.0f, 0.0f, 0.0f};
		const glm::vec3 size(scale, scale, scale);
		const float angle_step = glm::radians(360.0f / (float)children);
		glm::quat rot;
		scale *= 0.5f;
		for (float i = 0.0f; i < (float)children; ++i)
		{
			rot = glm::angleAxis(angle_step * i, glm::vec3{0.0f, 0.0f, 1.0f});
			transform = glm::scale(glm::translate(glm::toMat4(rot), pos), size);
			CreateBoxes(cube, model, vertex, fragment, scale, children, levels - 1, transform);
		}
	}
}

int main()
{
	Engine engine;
	flecs::world& world = engine.get_world();
	world.import<CameraComponents>();
	world.import<EventComponents>();
	world.import<GraphicsResourceComponents>();
	world.import<SceneComponents>();
	world.import<RendererComponents>();
	world.import<WindowComponents>();
	world.import<CameraSystems>();
	world.import<EventSystems>();
	world.import<GraphicsResourceSystems>();
	world.import<RendererSystems>();
	world.import<WindowSystems>();
	world.import<SceneSystems>();
	world.import<DebugModelRendererSystems>();

	Renderer::RendererConfig renderer{};
	renderer.multi_threaded = true;
	world.set<Renderer::RendererConfig>(renderer);
	world.add<Window::WindowConfig>();
	create_video(world);
	create_window(world);
	create_renderer(world);

	flecs::entity scene = world.entity()
						  .add<Scene::Root>()
						  .add<Scene::Node>()
						  .add<Scene::Translation>();

	flecs::entity camera = world.entity().child_of(scene)
						   .add<Scene::Node>()
						   .set<Scene::Translation>({glm::translate(glm::mat4(1.0f), {0.0f, 0.0f, -50.0f})})
						   .add<Camera::Eye>()
						   .set<Camera::Perspective>({glm::radians(45.0f), 0.01f, 100.0f})
						   .add<Camera::Viewport>();

	constexpr const char MODEL[] = "../Data/Models/Box.mdl";
	constexpr const char VERTEX_SHADER[] = "white_solid.vert";
	constexpr const char FRAGMENT_SHADER[] = "white_solid.frag";
	CreateBoxes(scene, MODEL, VERTEX_SHADER, FRAGMENT_SHADER, 1.0f, 8, 4);

	engine.Run();

	return 0;
}
