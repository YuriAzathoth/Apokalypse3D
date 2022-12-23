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

	onKeyDown_ = world.observer()
				 .term<const Process>()
				 .event<KeyDown>()
				 .each([](flecs::iter& it, size_t i)
	{
		const KeyDown* arg = it.param<KeyDown>();
		it.world().filter_builder<ActionKeyState>()
			.term<ActionKey>()
			.term<KeyboardBind>()
			.term(arg->keycode)
			.build()
			.each([](ActionKeyState& action)
			{
				action.current = true;
			});
	});

	onKeyUp_ = world.observer()
			   .term<const Process>()
			   .event<KeyUp>()
			   .each([](flecs::iter& it, size_t i)
	{
		const KeyUp* arg = it.param<KeyUp>();
		it.world().filter_builder<ActionKeyState>()
			.term<ActionKey>()
			.term<KeyboardBind>()
			.term(arg->keycode)
			.build()
			.each([](ActionKeyState& action)
			{
				action.current = false;
			});
	});

	updateKeyActions_ = world.system<ActionKeyState>("UpdateKeyActions")
						.kind(flecs::PreStore)
						.multi_threaded()
						.each([](ActionKeyState& state)
	{
		state.previous = state.current;
	});
}
