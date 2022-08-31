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

#include "SystemComponents.h"

using namespace A3D::Components::System;

namespace A3D
{
SystemComponents::SystemComponents(flecs::world& world)
{
	flecs::_::cpp_type<SystemComponents>::id_explicit(world, 0, false);
	world.module<SystemComponents>("A3D::Components::System");

	phase_ = world.component<Phase>()
		.constant("Begin", (unsigned)Phase::Begin)
		.constant("Prepare", (unsigned)Phase::Prepare)
		.constant("Submit", (unsigned)Phase::Submit)
		.constant("End", (unsigned)Phase::End);
	singleton_ = world.component<Singleton>().add(flecs::Tag);
	singletons_ = world.component<Singletons>();
	system_ = world.component<System>();

	world.set<Singletons>(
	{
		world.query_builder<>()
			.term<System>()
			.term<const Singleton>()
			.term<Phase>(Phase::Begin)
			.build(),
		world.query_builder<>()
			.term<System>()
			.term<const Singleton>()
			.term<Phase>(Phase::Prepare)
			.build(),
		world.query_builder<>()
			.term<System>()
			.term<const Singleton>()
			.term<Phase>(Phase::Submit)
			.build(),
		world.query_builder<>()
			.term<System>()
			.term<const Singleton>()
			.term<Phase>(Phase::End)
			.build()
	});
}
} // namespace A3D
