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

#include <SDL3/SDL.h>
#include "EventComponents.h"
#include "EventSystems.h"
#include "IO/Log.h"
#include "RawInputComponents.h"

using namespace A3D::Components::Event;
using namespace A3D::Components::RawInput;

static void poll_events(flecs::entity e, Process& process)
{
	LogTrace("SDL events processing begin...");
	flecs::world w = e.world();
	while (SDL_PollEvent(&process.event))
		switch (process.event.type)
		{
		case SDL_KEYDOWN:
			w.event<KeyDown>()
				.id<Process>()
				.entity(e)
				.ctx(KeyDown{static_cast<Key>(process.event.key.keysym.scancode)})
				.emit();
			break;
		case SDL_KEYUP:
			w.event<KeyUp>()
				.id<Process>()
				.entity(e)
				.ctx(KeyUp{static_cast<Key>(process.event.key.keysym.scancode)})
				.emit();
			break;
		case SDL_MOUSEBUTTONDOWN:
			w.event<MouseButtonDown>()
				.id<Process>()
				.entity(e)
				.ctx(MouseButtonDown{process.event.button.button})
				.emit();
			break;
		case SDL_MOUSEBUTTONUP:
			w.event<MouseButtonDown>()
				.id<Process>()
				.entity(e)
				.ctx(MouseButtonDown{process.event.button.button})
				.emit();
			break;
		case SDL_MOUSEMOTION:
			w.event<MouseMove>()
				.id<Process>()
				.entity(e)
				.ctx(MouseMove
				{
					process.event.motion.x,
					process.event.motion.y,
					process.event.motion.xrel,
					process.event.motion.yrel
				})
				.emit();
			break;
		case SDL_MOUSEWHEEL:
			w.event<MouseWheel>()
				.id<Process>()
				.entity(e)
				.ctx(MouseWheel{process.event.wheel.y})
				.emit();
			break;
		case SDL_QUIT:
			w.quit();
			break;
		}
	LogTrace("SDL events processing end...");
}

namespace A3D
{
EventSystems::EventSystems(flecs::world& world)
{
	world.module<EventSystems>("A3D::Systems::Event");
	world.import<EventComponents>();
	world.import<RawInputComponents>();

	onAddProcess_ = world.observer<Process>("Init")
					.event(flecs::OnAdd)
					.each([](Process&)
	{
		LogInfo("Initializing SDL events...");
		SDL_InitSubSystem(SDL_INIT_EVENTS);
		LogInfo("SDL events has been initialized...");
	});

	onRemoveProcess_ = world.observer<Process>("Destroy")
					   .event(flecs::OnRemove)
					   .each([](Process&)
	{
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
		LogInfo("SDL events shutdowned.");
	});

	pollEvents_ = world.system<Process>("PollEvents")
				  .kind(flecs::OnLoad)
				  .no_readonly()
				  .each(poll_events);

	world.add<Process>();
}
} // namespace A3D
