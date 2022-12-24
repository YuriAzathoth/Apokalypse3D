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
#include "InputComponents.h"
#include "InputSystems.h"
#include "IO/Log.h"
#include "KeyboardComponents.h"
#include "MouseComponents.h"

using namespace A3D::Components::Event;
using namespace A3D::Components::Input;
using namespace A3D::Components::Keyboard;

A3D::InputSystems::InputSystems(flecs::world& world)
{
	world.module<InputSystems>("A3D::Systems::Input");
	world.import<EventComponents>();
	world.import<InputComponents>();
	world.import<KeyboardComponents>();
	world.import<MouseComponents>();

	flecs::query keyActions = world.query<ActionKey, const KeyboardKey>();
	updateKeyboard_ = world.observer<const Keyboard>("UpdateKeyboard")
					  .event(flecs::OnSet)
					  .each([keyActions = std::move(keyActions)](flecs::entity e, const Keyboard& keyboard)
	{
		keyActions.each([&keyboard](flecs::entity e, ActionKey& action, const KeyboardKey& key)
		{
			const unsigned element_id = key.keycode / KEYS_PER_ELEMENT;
			const unsigned bit_shift = key.keycode % KEYS_PER_ELEMENT;
			action.current = keyboard.down[element_id] << (1 << bit_shift);
		});
	});

	updateKeyActions_ = world.system<ActionKey>("UpdateKeyActions")
						.kind(flecs::OnStore)
						.multi_threaded()
						.each([](ActionKey& action)
	{
		action.previous = action.current;
	});
}
