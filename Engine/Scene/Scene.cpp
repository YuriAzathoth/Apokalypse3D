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
#include "Scene.h"
#include "SceneComponents.h"

static void UpdateNode(flecs::entity e, const glm::mat4& parent)
{
	Node* node = e.get_mut<Node>();
	const Translation* trans = e.get<Translation>();
	if (node && trans)
	{
		node->model = parent * trans->model;
		e.children([node](flecs::entity e) { UpdateNode(e, node->model); });
	}
}

void RegisterScene(flecs::world& world)
{
	world.component<Node>();
	world.component<Scene>();
	world.component<Translation>();

	world.component<Look>();
	world.component<Move>();
	world.component<Rotate>();

	const glm::mat4 identity(1.0f);
	world.system<Scene>().each([identity](flecs::entity e, Scene) { UpdateNode(e, identity); });
}

/*#include <glm/gtx/quaternion.hpp>
#include "Scene.h"
#include "SceneComponents.h"

Scene::Scene(flecs::world& world, const char* sceneName)
{
	root_ = world.entity<Node>().add<Node>().add<Translation>();
}

Scene::~Scene()
{
	root_.destruct();
}

void Scene::InitSystems(flecs::world& world)
{
	world.system<Translation, const Rotate>().each([](flecs::iter& it, size_t, Translation& tr, const Rotate& rotate)
											 { tr.model *= glm::toMat4(glm::quat(rotate.rot * it.delta_time())); });

	world.system<Translation, const Move>().each([](flecs::iter& it, size_t, Translation& tr, const Move& move)
										   { tr.model = glm::translate(tr.model, move.move * it.delta_time()); });
}

void Scene::Update(flecs::entity& node, const glm::mat4& parent) noexcept
{
	Node* cnode = node.get_mut<Node>();
	const Translation* tr = node.get<Translation>();
	if (cnode && tr)
	{
		cnode->model = parent * tr->model;
		node.children([this, cnode](flecs::entity child) { Update(child, cnode->model); });
	}
}*/
