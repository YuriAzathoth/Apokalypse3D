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

#include "InputComponents.h"

using namespace A3D::Components::Input;

A3D::InputComponents::InputComponents(flecs::world& world)
{
	world.module<InputComponents>("A3D::Components::Input");

	actionAxis_ = world.component<ActionAxis>()
		.on_add([](ActionAxis& axis)
		{
			axis.delta = 0.0f;
		})
		.member<float>("delta");

	actionKey_ = world.component<ActionKey>()
		.on_add([](ActionKey& key)
		{
			key.current = false;
			key.previous = false;
		})
		.member<bool>("current")
		.member<bool>("previous");

	controller_ = world.component<Controller>().add(flecs::Tag);
	sensitivity_ = world.component<Sensitivity>().add(flecs::Tag);
	controllerAxis_ = world.component<ControllerAxis>().member<float>("delta");
	isAxisOf_ = world.component<IsAxisOf>().add(flecs::Tag).add(flecs::Acyclic);
	isAxisControlOf_ = world.component<IsAxisControlOf>().add(flecs::Tag).add(flecs::Acyclic);
}