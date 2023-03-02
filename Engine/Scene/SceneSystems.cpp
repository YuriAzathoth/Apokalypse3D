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

	resetRelativeTransform_ = world.system<RelativeTransform>("ResetRelativeTransform")
		.term<const Node>()
		.term<const Position>().or_()
		.term<const Rotation>().or_()
		.term<const Scale>().or_()
		.without<Static>()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt)
		{
			rt.local = glm::mat4{1.0f};
		});

	setPosition_ = world.system<RelativeTransform, const Position>("SetPosition")
		.without<Static>()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Position& pos)
		{
			rt.local = glm::translate(rt.local, pos.position);
		});

	setRotation_ = world.system<RelativeTransform, const Rotation>("SetRotation")
		.without<Static>()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Rotation& rot)
		{
			rt.local *= glm::toMat4(rot.quat);
		});

	setScale_ = world.system<RelativeTransform, const Scale>("SetScale")
		.without<Static>()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Scale& scale)
		{
			rt.local = glm::scale(rt.local, scale.scale);
		});

	buildTransform_ = world.system<WorldTransform, const RelativeTransform, const WorldTransform>("BuildTransform")
		.arg(3).parent().cascade()
		.without<Static>()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](WorldTransform& wt, const RelativeTransform& rt, const WorldTransform& parent)
		{
			wt.global = parent.global * rt.local;
		});
}
} // namespace A3D
