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
#include "KeyboardComponents.h"
#include "KeyboardSystems.h"

using namespace A3D::Components::Input;
using namespace A3D::Components::Keyboard;

A3D::KeyboardSystems::KeyboardSystems(flecs::world& world)
{
	world.module<KeyboardSystems>("A3D::Systems::Keyboard");
	world.import<InputComponents>();
	world.import<KeyboardComponents>();

	flecs::query keyActions = world.query<ActionKey, const KeyboardKey>();

	update_ = world.observer<const Keyboard>("Update")
			  .event(flecs::OnSet)
			  .each([keyActions = std::move(keyActions)](const Keyboard& keyboard)
	{
		keyActions.each([&keyboard](flecs::entity e, ActionKey& action, const KeyboardKey& key)
		{
			const unsigned element_id = key.keycode / KEYS_PER_ELEMENT;
			const unsigned bit_shift = key.keycode % KEYS_PER_ELEMENT;
			action.current = keyboard.down[element_id] & (1 << bit_shift);
			if (action.previous != action.current)
				e.modified<ActionKey>();
		});
	});

	world.add<Keyboard>();
}
