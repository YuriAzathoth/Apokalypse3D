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
#include "Input/KeyboardComponents.h"
#include "Input/KeyboardSystems.h"
#include "Input/MouseComponents.h"
#include "Input/MouseSystems.h"
#include "IO/AsyncLoaderComponents.h"
#include "Scene/SceneComponents.h"
#include "Scene/SceneSystems.h"

using namespace A3D;
using namespace A3D::Components;

struct Rotate
{
	float pitch;
	float yaw;
	float roll;
};

struct CamControl {};

void CreateBoxes(flecs::entity parent,
				 const char* model,
				 const char* vertex,
				 const char* fragment,
				 float roll,
				 float scale,
				 unsigned children,
				 unsigned levels,
				 bool root = true)
{
	flecs::entity cube;
	if (!root)
	{
		flecs::entity origin = parent.world().entity().child_of(parent)
			.add<Scene::Node>()
			.set<Scene::Rotation>({glm::angleAxis(roll, glm::vec3{0.0f, 0.0f, 1.0f})});

		cube = parent.world().entity().child_of(origin)
			.add<Scene::Node>()
			.set<Scene::Position>({{10.0f, 0.0f, 0.0f}})
			.set<Scene::Scale>({{scale, scale, scale}});
	}
	else
		cube = parent.world().entity().child_of(parent).add<Scene::Node>();

	cube.set<Mesh::GetModelFile>({model})
		.set<GpuProgram::GetProgram>({vertex, fragment})
		.set<Rotate>({glm::radians(50.0f), glm::radians(15.0f), glm::radians(25.0f)});

	if (levels > 1)
	{
		const float fl_children = (float)children;
		const float angle_step = glm::radians(360.0f / fl_children);
		scale *= 0.5f;
		for (float i = 0.0f; i < fl_children; ++i)
			CreateBoxes(cube, model, vertex, fragment, angle_step * i, scale, children, levels - 1, false);
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
	world.import<KeyboardSystems>();
	world.import<MouseSystems>();
	world.import<GpuProgramCacheSystems>();
	world.import<ImageCacheSystems>();
	world.import<MeshCacheSystems>();
	world.import<RendererSystems>();
	world.import<WindowSystems>();
	world.import<SceneSystems>();
	world.import<DebugModelRendererSystems>();

	Renderer::RendererConfig renderer{};
	renderer.fullscreen = false;
	renderer.width = 800;
	renderer.height = 600;
	renderer.multi_threaded = true;
	renderer.type = Renderer::RendererType::OpenGL;
	Window::WindowConfig window{};
	window.width = 800;
	window.height = 600;
	window.fullscreen = false;
	world.set<Renderer::RendererConfig>(renderer);
	world.set<Window::WindowConfig>({window});
	create_video(world);
	create_window(world);
	create_renderer(world);

	world.component<CamControl>().add(flecs::Tag).add(flecs::Acyclic);
	world.component<Rotate>();

	flecs::entity scene = world.entity().add<Scene::Root>();

	flecs::entity camera = world.entity().child_of(scene)
						   .add<Scene::Node>()
						   .set<Scene::Position>({{0.0f, 0.0f, 50.0f}})
						   .add<Camera::Eye>()
						   .set<Camera::Perspective>({glm::radians(45.0f), 0.01f, 1000.0f})
						   .add<Camera::Viewport>();

	constexpr const char MODEL[] = "../Data/Models/Box.mdl";
	constexpr const char VERTEX_SHADER[] = "white_solid.vert";
	constexpr const char FRAGMENT_SHADER[] = "white_solid.frag";
	CreateBoxes(scene, MODEL, VERTEX_SHADER, FRAGMENT_SHADER, 0.0f, 1.0f, 8, 4);

	world.system<Scene::Rotation, const Rotate>()
		.multi_threaded()
		.each([](flecs::entity e, Scene::Rotation& rotation, const Rotate& rotate)
		{
			const glm::quat pitch = glm::angleAxis(rotate.pitch * e.delta_time(), glm::vec3{1.0f, 0.0f, 0.0f});
			const glm::quat yaw = glm::angleAxis(rotate.yaw * e.delta_time(), glm::vec3{0.0f, 1.0f, 0.0f});
			const glm::quat roll = glm::angleAxis(rotate.roll * e.delta_time(), glm::vec3{0.0f, 0.0f, 1.0f});
			rotation.quat *= pitch * yaw * roll;
		});

	flecs::entity action_exit = world.entity();
	world.entity("ACTION_EXIT")
		.add(action_exit)
		.add<Input::ActionKey>()
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_ESCAPE});
	world.system<Input::ActionKey>()
		.term(action_exit)
		.each([](flecs::entity e, Input::ActionKey& action)
		{
			if (action.current)
				e.world().quit();
		});

	flecs::entity forward_back = world.entity();
	flecs::entity left_right = world.entity();
	flecs::entity up_down = world.entity();
	flecs::entity movement = world.entity().add<Input::Controller>();
	flecs::entity fbctrl = world.entity().add<Input::ControllerAxis>().add<Input::IsAxisOf>(movement).add(forward_back).add<CamControl>(camera);
	flecs::entity lrctrl = world.entity().add<Input::ControllerAxis>().add<Input::IsAxisOf>(movement).add(left_right).add<CamControl>(camera);
	flecs::entity udctrl = world.entity().add<Input::ControllerAxis>().add<Input::IsAxisOf>(movement).add(up_down).add<CamControl>(camera);

	world.entity("ACTION_FORWARD")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(fbctrl)
		.set<Input::Sensitivity>({-1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_W});
	world.entity("ACTION_BACK")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(fbctrl)
		.set<Input::Sensitivity>({1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_S});
	world.entity("ACTION_LEFT")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(lrctrl)
		.set<Input::Sensitivity>({-1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_A});
	world.entity("ACTION_RIGHT")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(lrctrl)
		.set<Input::Sensitivity>({1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_D});
	world.entity("ACTION_UP")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(udctrl)
		.set<Input::Sensitivity>({1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_SPACE});
	world.entity("ACTION_DOWN")
		.add<Input::ActionKey>()
		.add<Input::IsAxisControlOf>(udctrl)
		.set<Input::Sensitivity>({-1.0f})
		.set<Keyboard::KeyboardKey>({SDL_SCANCODE_LCTRL});

	world.system<Scene::Position, const Input::ControllerAxis>()
		.arg(1).up<CamControl>()
		.term(forward_back)
		.each([](flecs::entity e, Scene::Position& pos, const Input::ControllerAxis& axis)
	{
		pos.position.z += axis.delta * e.delta_time() * 50.0f;
	});

	world.system<Scene::Position, const Input::ControllerAxis>()
		.arg(1).up<CamControl>()
		.term(left_right)
		.each([](flecs::entity e, Scene::Position& pos, const Input::ControllerAxis& axis)
	{
		pos.position.x += axis.delta * e.delta_time() * 50.0f;
	});

	world.system<Scene::Position, const Input::ControllerAxis>()
		.arg(1).up<CamControl>()
		.term(up_down)
		.each([](flecs::entity e, Scene::Position& pos, const Input::ControllerAxis& axis)
	{
		pos.position.y += axis.delta * e.delta_time() * 50.0f;
	});

	flecs::entity look_pitch = world.entity();
	flecs::entity look_yaw = world.entity();
	flecs::entity look = world.entity().add<Input::Controller>();
	flecs::entity pitchctrl = world.entity("LOOK_PITCH")
		.add<Input::ControllerAxis>()
		.add<Input::IsAxisOf>(look)
		.add(look_pitch)
		.add<CamControl>(camera);
	flecs::entity yawctrl = world.entity("LOOK_YAW")
		.add<Input::ControllerAxis>()
		.add<Input::IsAxisOf>(look)
		.add(look_yaw)
		.add<CamControl>(camera);

	world.entity()
		.add<Input::ActionAxis>()
		.add<Input::IsAxisControlOf>(yawctrl)
		.add<Mouse::AxisX>()
		.set<Input::Sensitivity>({0.25f});
	world.entity()
		.add<Input::ActionAxis>()
		.add<Input::IsAxisControlOf>(pitchctrl)
		.add<Mouse::AxisY>()
		.set<Input::Sensitivity>({0.25f});

	world.system<Scene::Rotation, const Input::ControllerAxis>()
		.arg(1).up<CamControl>()
		.term(look_pitch)
		.each([](flecs::entity e, Scene::Rotation& rot, const Input::ControllerAxis& axis)
	{
		rot.quat *= glm::angleAxis(axis.delta * e.delta_time(), glm::vec3{-1.0f, 0.0f, 0.0f});
	});

	world.system<Scene::Rotation, const Input::ControllerAxis>()
		.arg(1).up<CamControl>()
		.term(look_yaw)
		.each([](flecs::entity e, Scene::Rotation& rot, const Input::ControllerAxis& axis)
	{
		rot.quat *= glm::angleAxis(axis.delta * e.delta_time(), glm::vec3{0.0f, -1.0f, 0.0f});
	});

	engine.Run();

	return 0;
}
