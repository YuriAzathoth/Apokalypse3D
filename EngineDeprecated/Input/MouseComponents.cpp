/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2023 Yuriy Zinchenko

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

#include "MouseComponents.h"

using namespace A3D::Components::Mouse;

A3D::MouseComponents::MouseComponents(flecs::world& world)
{
	world.module<MouseComponents>("A3D::Components::Mouse");

	axisX_ = world.component<AxisX>().add(flecs::Tag);
	axisY_ = world.component<AxisY>().add(flecs::Tag);
	wheel_ = world.component<Wheel>().add(flecs::Tag);
	button_ = world.component<Button>().member<unsigned char>("code");

	buttonsState_ = world.component<ButtonsState>().member<unsigned char>("down");
	movement_ = world.component<Movement>().member<float>("dx").member<float>("dy");
	wheelState_ = world.component<WheelState>().member<int>("delta");
}
