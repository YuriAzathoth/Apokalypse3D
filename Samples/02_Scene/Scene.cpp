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
#include <glm/glm.hpp>
#include "Core/Engine.h"
#include "Debug/DebugModelRendererSystems.h"
#include "Graphics/CameraComponents.h"
#include "Graphics/CameraSystems.h"
#include "Graphics/GpuProgramCacheComponents.h"
#include "Graphics/GpuProgramCacheSystems.h"
#include "Graphics/GpuProgramComponents.h"
#include "Graphics/ImageCacheComponents.h"
#include "Graphics/ImageCacheSystems.h"
#include "Graphics/ImageComponents.h"
#include "Graphics/MeshCacheComponents.h"
#include "Graphics/MeshCacheSystems.h"
#include "Graphics/MeshComponents.h"
#include "Graphics/RendererComponents.h"
#include "Graphics/RendererSystems.h"
#include "Graphics/WindowComponents.h"
#include "Graphics/WindowSystems.h"
#include "Input/EventComponents.h"
#include "Input/EventSystems.h"
#include "Input/InputComponents.h"
#include "Input/InputSystems.h"
#include "Input/RawInputComponents.h"
#include "IO/AsyncLoaderComponents.h"
#include "Scene/SceneComponents.h"
#include "Scene/SceneSystems.h"

using namespace A3D;
using namespace A3D::Components;

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
						 .set<Mesh::GetModelFile>({model})
						 .set<GpuProgram::GetProgram>({vertex, fragment});

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
	world.import<AsyncLoaderComponents>();
	world.import<CameraComponents>();
	world.import<EventComponents>();
	world.import<InputComponents>();
	world.import<GpuProgramComponents>();
	world.import<GpuProgramCacheComponents>();
	world.import<ImageComponents>();
	world.import<ImageCacheComponents>();
	world.import<MeshComponents>();
	world.import<MeshCacheComponents>();
	world.import<SceneComponents>();
	world.import<RendererComponents>();
	world.import<WindowComponents>();
	world.import<CameraSystems>();
	world.import<EventSystems>();
	world.import<InputSystems>();
	world.import<GpuProgramCacheSystems>();
	world.import<ImageCacheSystems>();
	world.import<MeshCacheSystems>();
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
						   .set<Camera::Perspective>({glm::radians(45.0f), 0.01f, 1000.0f})
						   .add<Camera::Viewport>();

	constexpr const char MODEL[] = "../Data/Models/Box.mdl";
	constexpr const char VERTEX_SHADER[] = "white_solid.vert";
	constexpr const char FRAGMENT_SHADER[] = "white_solid.frag";
	CreateBoxes(scene, MODEL, VERTEX_SHADER, FRAGMENT_SHADER, 1.0f, 8, 4);

	flecs::entity action_exit = world.entity();
	world.entity("ACTION_EXIT")
		.add(action_exit)
		.add<Input::ActionKey>().set<Input::ActionKeyState>({})
		.add<Input::KeyboardBind>().add(RawInput::Key::KEY_ESCAPE);
	world.system<Input::ActionKeyState>()
		.term(action_exit)
		.each([](flecs::entity e, Input::ActionKeyState& action)
		{
			if (action.current)
				e.world().quit();
		});

	engine.Run();

	return 0;
}
