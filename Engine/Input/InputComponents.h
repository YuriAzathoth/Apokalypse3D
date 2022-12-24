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

#ifndef INPUTCOMPONENTS_H
#define INPUTCOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace RawInput { enum class Key : unsigned; }

namespace Input
{
struct ActionAxis
{
	float delta;
};

struct ActionKey
{
	bool current;
	bool previous;
};

struct Controller {};

struct ControllerAxis
{
	float delta;
};

struct Sensitivity
{
	float sensitivity;
};

struct IsAxisOf {};
struct IsAxisControlOf {};
} // namespace Input
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT InputComponents
{
	explicit InputComponents(flecs::world& world);

	flecs::entity actionAxis_;
	flecs::entity actionKey_;

	flecs::entity controller_;
	flecs::entity controllerAxis_;
	flecs::entity sensitivity_;
	flecs::entity isAxisOf_;
	flecs::entity isAxisControlOf_;
};
} // namespace A3D


#endif // INPUTCOMPONENTS_H
