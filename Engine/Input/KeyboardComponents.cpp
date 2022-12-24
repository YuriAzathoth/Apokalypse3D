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

#include "KeyboardComponents.h"

using namespace A3D::Components::Keyboard;

A3D::KeyboardComponents::KeyboardComponents(flecs::world& world)
{
	world.module<KeyboardComponents>("A3D::Components::Keyboard");

	keyboard_ = world.component<Keyboard>()
				.on_add([](Keyboard& keyboard)
				{
					constexpr const unsigned size = static_cast<unsigned>(sizeof(keyboard.down) / sizeof(unsigned));
					for (unsigned i = 0; i < size; ++i)
						keyboard.down[i] = 0;
				});

	keyboardKey_ = world.component<KeyboardKey>().member<unsigned>("keycode");
}
