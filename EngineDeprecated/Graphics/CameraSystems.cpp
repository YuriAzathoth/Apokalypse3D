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

#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
#include "CameraComponents.h"
#include "CameraSystems.h"
#include "GraphicsComponents.h"
#include "IO/Log.h"
#include "Scene/SceneComponents.h"

using namespace A3D::Components::Camera;
using namespace A3D::Components::Graphics;
using namespace A3D::Components::Scene;

A3D::CameraSystems::CameraSystems(flecs::world& world)
{
	world.module<CameraSystems>("A3D::Systems::Camera");
	world.import<CameraComponents>();
	world.import<GraphicsComponents>();
	world.import<SceneComponents>();

	view_ = world.system<Eye, const WorldTransform>("View")
			.kind(flecs::OnStore)
			.multi_threaded()
			.each([](Eye& eye, const WorldTransform& wt)
	{
		LogTrace("Set camera view...");
		glm::vec3 scale;
		glm::quat rotation;
		glm::vec3 position;
		glm::vec3 skew;
		glm::vec4 perspective;
		glm::decompose(wt.global, scale, rotation, position, skew, perspective);
		const glm::vec3 FRONT_VEC(0.0f, 0.0f, -1.0f);
		const glm::vec3 UP_VEC(0.0f, 1.0f, 0.0f);
		const glm::vec3 front = glm::normalize(rotation * FRONT_VEC);
		const glm::vec3 up = glm::normalize(rotation * UP_VEC);
		eye.view = glm::lookAt(position, position + front, up);
	});

	perspective_ = world.system<Eye, const Perspective, const Aspect>("Perspective")
				   .kind(flecs::OnStore)
				   .arg(3).singleton()
				   .multi_threaded()
				   .each([](Eye& eye, const Perspective& persp, const Aspect& aspect)
	{
		LogTrace("Set camera perspective...");
		eye.proj = glm::perspective(persp.fov, aspect.ratio, persp.nearest, persp.farthest);
	});
}
