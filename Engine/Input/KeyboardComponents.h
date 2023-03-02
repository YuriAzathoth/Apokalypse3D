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

#ifndef KEYBOARDCOMPONENTS_H
#define KEYBOARDCOMPONENTS_H

#include <bitset>
#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Keyboard
{
constexpr const unsigned KEYS_COUNT = 256;
constexpr const unsigned KEYS_PER_ELEMENT = static_cast<unsigned>(sizeof(unsigned)) * 8;
constexpr const unsigned KEYS_ARR_SIZE = KEYS_COUNT / KEYS_PER_ELEMENT;

struct Keyboard
{
	unsigned* down; // No need to copy 64 ints every frame.
};

struct KeyboardKey
{
	unsigned keycode;
};
} // namespace Keyboard
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT KeyboardComponents
{
	explicit KeyboardComponents(flecs::world& world);

	flecs::entity keyboard_;
	flecs::entity keyboardKey_;
};
} // namespace A3D


#endif // KEYBOARDCOMPONENTS_H
