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

	key_ = world.component<Key>()
		.constant("KEY_UNKNOWN", (int)Key::KEY_UNKNOWN)
		.constant("KEY_A", (int)Key::KEY_A)
		.constant("KEY_B", (int)Key::KEY_B)
		.constant("KEY_C", (int)Key::KEY_C)
		.constant("KEY_D", (int)Key::KEY_D)
		.constant("KEY_E", (int)Key::KEY_E)
		.constant("KEY_F", (int)Key::KEY_F)
		.constant("KEY_G", (int)Key::KEY_G)
		.constant("KEY_H", (int)Key::KEY_H)
		.constant("KEY_I", (int)Key::KEY_I)
		.constant("KEY_J", (int)Key::KEY_J)
		.constant("KEY_K", (int)Key::KEY_K)
		.constant("KEY_L", (int)Key::KEY_L)
		.constant("KEY_M", (int)Key::KEY_M)
		.constant("KEY_N", (int)Key::KEY_N)
		.constant("KEY_O", (int)Key::KEY_O)
		.constant("KEY_P", (int)Key::KEY_P)
		.constant("KEY_Q", (int)Key::KEY_Q)
		.constant("KEY_R", (int)Key::KEY_R)
		.constant("KEY_S", (int)Key::KEY_S)
		.constant("KEY_T", (int)Key::KEY_T)
		.constant("KEY_U", (int)Key::KEY_U)
		.constant("KEY_V", (int)Key::KEY_V)
		.constant("KEY_W", (int)Key::KEY_W)
		.constant("KEY_X", (int)Key::KEY_X)
		.constant("KEY_Y", (int)Key::KEY_Y)
		.constant("KEY_Z", (int)Key::KEY_Z)
		.constant("KEY_1", (int)Key::KEY_1)
		.constant("KEY_2", (int)Key::KEY_2)
		.constant("KEY_3", (int)Key::KEY_3)
		.constant("KEY_4", (int)Key::KEY_4)
		.constant("KEY_5", (int)Key::KEY_5)
		.constant("KEY_6", (int)Key::KEY_6)
		.constant("KEY_7", (int)Key::KEY_7)
		.constant("KEY_8", (int)Key::KEY_8)
		.constant("KEY_9", (int)Key::KEY_9)
		.constant("KEY_0", (int)Key::KEY_0)
		.constant("KEY_RETURN", (int)Key::KEY_RETURN)
		.constant("KEY_ESCAPE", (int)Key::KEY_ESCAPE)
		.constant("KEY_BACKSPACE", (int)Key::KEY_BACKSPACE)
		.constant("KEY_TAB", (int)Key::KEY_TAB)
		.constant("KEY_SPACE", (int)Key::KEY_SPACE)
		.constant("KEY_LCTRL", (int)Key::KEY_LCTRL)
		.constant("KEY_LSHIFT", (int)Key::KEY_LSHIFT)
		.constant("KEY_LALT", (int)Key::KEY_LALT)
		.constant("KEY_LGUI", (int)Key::KEY_LGUI)
		.constant("KEY_RCTRL", (int)Key::KEY_RCTRL)
		.constant("KEY_RSHIFT", (int)Key::KEY_RSHIFT)
		.constant("KEY_RALT", (int)Key::KEY_RALT)
		.constant("KEY_RGUI", (int)Key::KEY_RGUI)
		;

	keyDown_ = world.component<KeyDown>().member<Key>("code");
	keyUp_ = world.component<KeyUp>().member<Key>("code");

	mouseButtonDown_ = world.component<MouseButtonDown>().member<unsigned char>("button");
	mouseButtonUp_ = world.component<MouseButtonDown>().member<unsigned char>("button");
	mouseMove_ = world.component<MouseMove>()
		.member<int>("posx")
		.member<int>("posy")
		.member<int>("dx")
		.member<int>("dy");
	mouseWheel_ = world.component<MouseWheel>().member<int>("dist");
}
