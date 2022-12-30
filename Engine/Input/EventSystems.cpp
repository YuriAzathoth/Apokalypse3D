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
#include "KeyboardComponents.h"
#include "MouseComponents.h"

using namespace A3D::Components::Event;
using namespace A3D::Components::Keyboard;
using namespace A3D::Components::Mouse;

static void poll_events(flecs::iter& it,
						size_t,
						Keyboard* keyboard,
						ButtonsState* mouseButtons,
						Movement* mouseMove,
						WheelState* mouseWheel)
{
	LogTrace("SDL events processing begin...");
	flecs::world w = it.world();
	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_KEYDOWN:
			if (keyboard)
			{
				const unsigned element_id = static_cast<unsigned>(event.key.keysym.scancode) / KEYS_PER_ELEMENT;
				const unsigned bit_shift = static_cast<unsigned>(event.key.keysym.scancode) % KEYS_PER_ELEMENT;
				keyboard->down[element_id] |= (1 << bit_shift);
				w.modified<Keyboard>();
			}
			break;
		case SDL_KEYUP:
			if (keyboard)
			{
				const unsigned element_id = static_cast<unsigned>(event.key.keysym.scancode) / KEYS_PER_ELEMENT;
				const unsigned bit_shift = static_cast<unsigned>(event.key.keysym.scancode) % KEYS_PER_ELEMENT;
				keyboard->down[element_id] &= ~(1 << bit_shift);
				w.modified<Keyboard>();
			}
			break;
		case SDL_MOUSEBUTTONDOWN:
			if (mouseButtons)
			{
				const unsigned bit_shift = static_cast<unsigned>(event.button.button);
				mouseButtons->down |= (1 << bit_shift);
				w.modified<ButtonsState>();
			}
			break;
		case SDL_MOUSEBUTTONUP:
			if (mouseButtons)
			{
				const unsigned bit_shift = static_cast<unsigned>(event.button.button);
				mouseButtons->down &= ~(1 << bit_shift);
				w.modified<ButtonsState>();
			}
			break;
		case SDL_MOUSEMOTION:
			if (mouseMove)
			{
				mouseMove->dx = event.motion.xrel;
				mouseMove->dy = event.motion.yrel;
				w.modified<Movement>();
			}
			break;
		case SDL_MOUSEWHEEL:
			if (mouseWheel)
			{
				mouseWheel->delta = event.wheel.y;
				w.modified<WheelState>();
			}
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
	world.import<KeyboardComponents>();
	world.import<MouseComponents>();

	onAddProcess_ = world.observer<>("Init")
					.term<Process>()
					.event(flecs::OnAdd)
					.each([](flecs::iter, size_t)
	{
		LogDebug("Initializing SDL events...");
		SDL_InitSubSystem(SDL_INIT_EVENTS);
		LogInfo("SDL events has been initialized.");
	});

	onRemoveProcess_ = world.observer<>("Destroy")
					   .term<Process>()
					   .event(flecs::OnRemove)
					   .each([](flecs::iter, size_t)
	{
		SDL_QuitSubSystem(SDL_INIT_EVENTS);
		LogInfo("SDL events shutdowned.");
	});

	pollEvents_ = world.system<Keyboard*, ButtonsState*, Movement*, WheelState*>("PollEvents")
				  .with<Process>().singleton()
				  .arg(2).singleton()
				  .arg(3).singleton()
				  .arg(4).singleton()
				  .arg(5).singleton()
				  .kind(flecs::OnLoad)
				  .no_readonly()
				  .each(poll_events);

	world.add<Process>();
}
} // namespace A3D
