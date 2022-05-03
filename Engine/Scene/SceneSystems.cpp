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

#define GLM_FORCE_LEFT_HANDED

#include <SDL2/SDL.h>
#include <flecs.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "Core/Coordinator.h"
#include "SceneSystems.h"

void RegisterSceneSystems(flecs::world& world, Coordinator& core)
{
	world.system<Translation, const Rotate>().each([](flecs::iter& it, size_t, Translation& tr, const Rotate& rotate)
											 { tr.model *= glm::toMat4(glm::quat(rotate.rot * it.delta_time())); });

	world.system<Translation, const Move>().each([](flecs::iter& it, size_t, Translation& tr, const Move& move)
										   { tr.model = glm::translate(tr.model, move.move * it.delta_time()); });
}

void UpdateScene(flecs::entity node, const Node& parent) noexcept
{
	Node* cnode = node.get_mut<Node>();
	const Translation* tr = node.get<Translation>();
	if (cnode && tr)
	{
		cnode->model = parent.model * tr->model;
		node.children([cnode](flecs::entity child) { UpdateScene(child, *cnode); });
	}
}
