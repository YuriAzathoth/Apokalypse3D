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

#include "EventComponents.h"
#include "EventSystems.h"
#include "IO/Log.h"

using namespace A3D::Components::Event;

static void poll_events(flecs::entity e, EventProcessor& processor)
{
	LogTrace("SDL events processing begin...");
	SDL_Event& event = processor.event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			e.world().quit();
		}
	LogTrace("SDL events processing end...");
}

namespace A3D
{
EventSystems::EventSystems(flecs::world& world)
{
	flecs::_::cpp_type<EventSystems>::id_explicit(world, 0, false);
	world.import<EventComponents>();
	world.module<EventSystems>("A3D::Systems::Event");

	world.add<EventProcessor>();

	pollEvents_ = world.system<EventProcessor>("PollEvents")
				  .kind(flecs::OnLoad)
				  .each(poll_events);
}
} // namespace A3D
