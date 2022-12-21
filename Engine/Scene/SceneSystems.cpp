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

	resetRelativeTransform_ = world.system<RelativeTransform>("ResetRelativeTransform")
		.term<const Node>()
		.term<const Position>().or_()
		.term<const Rotation>().or_()
		.term<const Scale>().or_()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt)
		{
			rt.transform = glm::mat4{1.0f};
		});

	setPosition_ = world.system<RelativeTransform, const Position>("SetPosition")
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Position& pos)
		{
			rt.transform = glm::translate(rt.transform, pos.position);
		});

	setRotation_ = world.system<RelativeTransform, const Rotation>("SetRotation")
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Rotation& rot)
		{
			rt.transform *= glm::toMat4(rot.quat);
		});

	setScale_ = world.system<RelativeTransform, const Scale>("SetScale")
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](RelativeTransform& rt, const Scale& scale)
		{
			rt.transform = glm::scale(rt.transform, scale.scale);
		});

	buildTransform_ = world.system<WorldTransform, const RelativeTransform, const WorldTransform>("BuildTransform")
		.arg(3).parent().cascade()
		.kind(flecs::PostUpdate)
		.multi_threaded()
		.each([](WorldTransform& current, const RelativeTransform& relative, const WorldTransform& parent)
		{
			current.transform = parent.transform * relative.transform;
		});

	initPosition_ = world.observer<Position>("InitPosition")
		.event(flecs::OnAdd)
		.each([](Position& pos)
		{
			pos.position.x = 0.0f;
			pos.position.y = 0.0f;
			pos.position.z = 0.0f;
		});

	initRotate_ = world.observer<Rotation>("InitRotate")
		.event(flecs::OnAdd)
		.each([](Rotation& rot)
		{
			rot.quat.w = 1.0f;
			rot.quat.x = 0.0f;
			rot.quat.y = 0.0f;
			rot.quat.z = 0.0f;
		});

	initScale_ = world.observer<Scale>("InitScale")
		.event(flecs::OnAdd)
		.each([](Scale& scale)
		{
			scale.scale.x = 1.0f;
			scale.scale.y = 1.0f;
			scale.scale.z = 1.0f;
		});

	initRoot_ = world.observer<WorldTransform>("InitRoot")
		.term<const Root>()
		.event(flecs::OnAdd)
		.each([](WorldTransform& wt)
		{
			wt.transform = glm::mat4{1.0f};
		});
}
} // namespace A3D
