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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "SceneSystems.h"

void RegisterSceneSystems(flecs::world& world)
{
	world.system<Translation, Rotate>().each([](flecs::iter& it, size_t, Translation& tr, const Rotate& rotate)
											 { tr.model_ *= glm::toMat4(glm::quat(rotate.rot_ * it.delta_time())); });

	world.system<Translation, Move>().each([](flecs::iter& it, size_t, Translation& tr, const Move& move)
										   { glm::translate(tr.model_, move.move_) * it.delta_time(); });

	world.system<Camera>().each(
		[](const Camera& camera)
		{ bgfx::setViewTransform(0, glm::value_ptr(camera.view_), glm::value_ptr(camera.proj_)); });

	world.system<Camera, Ray>().each(
		[](Camera& camera, const Ray& ray) {
			camera.view_ = glm::lookAt(ray.begin_, ray.end_, {0.0f, 1.0f, 0.0f});
		});

	world.system<Camera, Perspective>().each(
		[](Camera& camera, const Perspective& perspective) {
			camera.proj_ = glm::perspective(perspective.fov_, perspective.aspect_, perspective.near_, perspective.far_);
		});
}

void UpdateScene(flecs::entity node, const Node& parent) noexcept
{
	Node* cnode = node.get_mut<Node>();
	const Translation* tr = node.get<Translation>();
	if (cnode && tr)
	{
		cnode->model_ = parent.model_ * tr->model_;
		node.children([cnode](flecs::entity child) { UpdateScene(child, *cnode); });
	}
}
