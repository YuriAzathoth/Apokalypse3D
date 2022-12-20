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

#include "Core/GlmComponents.h"
#include "SceneComponents.h"

using namespace A3D::Components::Scene;

namespace A3D
{
SceneComponents::SceneComponents(flecs::world& world)
{
	world.module<SceneComponents>("A3D::Components::Scene");
	world.import<GlmComponents>();

	relativeTransform_ = world.component<RelativeTransform>();
	worldTransform_ = world.component<WorldTransform>();//.add(flecs::With, changed_);

	position_ = world.component<Position>().member<glm::vec3>("position").add(flecs::With, relativeTransform_);
	rotation_ = world.component<Rotation>().member<glm::vec3>("euler").add(flecs::With, relativeTransform_);
	scale_ = world.component<Scale>().member<glm::vec3>("scale").add(flecs::With, relativeTransform_);

	node_ = world.component<Node>().add(flecs::Tag)
		.add(flecs::With, position_)
		.add(flecs::With, rotation_)
		.add(flecs::With, scale_)
		.add(flecs::With, worldTransform_);

	root_ = world.component<Root>().add(flecs::Tag).add(flecs::With, node_);
}
} // namespace A3D
