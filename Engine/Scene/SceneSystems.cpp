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
#include "IO/Log.h"
#include "SceneComponents.h"
#include "SceneSystems.h"

using namespace A3D::Components::Scene;

namespace A3D
{
SceneSystems::SceneSystems(flecs::world& world)
{
	world.module<SceneSystems>("A3D::Systems::Scene");
	world.import<SceneComponents>();

	move_ = world.system<Translation, const Move>("Move")
		.multi_threaded()
		.each([](flecs::iter& it, size_t, Translation& tr, const Move& move)
		{
			LogTrace("Moving scene node...");
			tr.model = glm::translate(tr.model, move.move * it.delta_time());
		});

	rotate_ = world.system<Translation, const Rotate>("Rotate")
		.multi_threaded()
		.each([](flecs::iter& it, size_t, Translation& tr, const Rotate& rotate)
		{
			LogTrace("Rotating scene node...");
			tr.model *= glm::toMat4(glm::quat(rotate.rot * it.delta_time()));
		});

	updateScene_ = world.system<Node, const Translation, const Node>("Update")
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.arg(3).parent().cascade()
		.each([](Node& node, const Translation& translation, const Node& parent)
		{
			LogTrace("Updating hierarchy child matrix...");
			node.model = parent.model * translation.model;
		});
}
} // namespace A3D
