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
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "IO/Log.h"
#include "RendererComponents.h"
#include "Scene/SceneComponents.h"

using namespace A3D::Components::Renderer;
using namespace A3D::Components::Camera;
using namespace A3D::Components::Scene;

namespace A3D
{
CameraSystems::CameraSystems(flecs::world& world)
{
	world.module<CameraSystems>("A3D::Systems::Camera");
	world.import<CameraComponents>();
	world.import<RendererComponents>();
	world.import<SceneComponents>();

	view_ = world.system<Eye, const Node>("View")
			.kind(flecs::OnStore)
			.each([](Eye& eye, const Node& node)
	{
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(node.model, scale, rotation, position, skew, perspective);
		const glm::vec3 FRONT_VEC(0.0f, 0.0f, -1.0f);
		const glm::vec3 UP_VEC(0.0f, 1.0f, 0.0f);
		const glm::vec3 front = rotation * FRONT_VEC;
		const glm::vec3 up = rotation * UP_VEC;
		const glm::vec3 target = position + front;
		eye.view = glm::lookAt(target, position, up);
	});

	perspective_ = world.system<Eye, const Perspective, const Aspect>("Perspective")
				   .kind(flecs::OnStore)
				   .arg(3).singleton()
				   .each([](Eye& eye, const Perspective& persp, const Aspect& aspect)
	{
		eye.proj = glm::perspective(persp.fov, aspect.ratio, persp.nearest, persp.farthest);
	});

	update_ = world.system<const Eye>("CameraUpdate")
			  .kind(flecs::OnStore)
			  .each([](const Eye& eye)
	{
		bgfx::setViewTransform(0, glm::value_ptr(eye.view), glm::value_ptr(eye.proj));
	});
}
} // namespace A3D
