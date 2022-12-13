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

#include "RawInputComponents.h"

using namespace A3D::Components::RawInput;

A3D::RawInputComponents::RawInputComponents(flecs::world& world)
{
	world.module<RawInputComponents>("A3D::Components::RawInput");

	keyDown_ = world.component<KeyDown>().member<unsigned>("sym");
	keyUp_ = world.component<KeyUp>().member<unsigned>("sym");

	mouseButtonDown_ = world.component<MouseButtonDown>().member<unsigned char>("button");
	mouseButtonUp_ = world.component<MouseButtonDown>().member<unsigned char>("button");
	mouseMove_ = world.component<MouseMove>()
		.member<int>("posx")
		.member<int>("posy")
		.member<int>("dx")
		.member<int>("dy");
	mouseWheel_ = world.component<MouseWheel>().member<int>("dist");
}
