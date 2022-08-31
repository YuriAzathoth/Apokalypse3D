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

#include "Core/SystemComponents.h"
#include "EventComponents.h"
#include "EventSystems.h"
#include "IO/Log.h"

using namespace A3D::Components::Event;
using namespace A3D::Components::System;

namespace A3D
{
static void PollEvents(flecs::world& world)
{
	LogTrace("SDL events processing begin...");
	SDL_Event& event = world.get_mut<EventProcessor>()->event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			world.quit();
		}
	LogTrace("SDL events processing end...");
}

EventSystems::EventSystems(flecs::world& world)
{
	flecs::_::cpp_type<EventSystems>::id_explicit(world, 0, false);
	world.import<EventComponents>();
	world.module<EventSystems>("A3D::Systems::Event");

	world.add<EventProcessor>();
	world.entity().set<System>({PollEvents}).add<Singleton>().add(Phase::Begin);
}
} // namespace A3D
