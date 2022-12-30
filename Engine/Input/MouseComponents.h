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

#ifndef MOUSECOMPONENTS_H
#define MOUSECOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Mouse
{
struct AxisX {};
struct AxisY {};
struct Wheel {};

struct Button
{
	unsigned char code;
};

struct ButtonsState
{
	unsigned char down;

	ButtonsState() : down(0) {}
	ButtonsState(const ButtonsState& src) : down(src.down) {}
	void operator=(const ButtonsState& src) { down = src.down; }
};

struct Movement
{
	float dx;
	float dy;

	Movement() : dx(0.0f), dy(0.0f) {}
	Movement(const Movement& src) : dx(src.dx), dy(src.dy) {}
	void operator=(const Movement& src) { dx = src.dx; dy = src.dy; }
};

struct WheelState
{
	float delta;

	WheelState() : delta(0.0f) {}
	WheelState(const WheelState& src) : delta(src.delta) {}
	void operator=(const WheelState& src) { delta = src.delta; }
};
} // namespace Mouse
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT MouseComponents
{
	explicit MouseComponents(flecs::world& world);

	flecs::entity axisX_;
	flecs::entity axisY_;
	flecs::entity button_;
	flecs::entity buttonsState_;
	flecs::entity movement_;
	flecs::entity wheel_;
	flecs::entity wheelState_;
};
} // namespace A3D


#endif // MOUSECOMPONENTS_H
