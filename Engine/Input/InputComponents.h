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
#include <SDL3/SDL_scancode.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Input
{
enum Key : unsigned
{
	KEY_UNKNOWN = SDL_SCANCODE_UNKNOWN,
	KEY_A = SDL_SCANCODE_A,
	KEY_B = SDL_SCANCODE_B,
	KEY_C = SDL_SCANCODE_C,
	KEY_D = SDL_SCANCODE_D,
	KEY_E = SDL_SCANCODE_E,
	KEY_F = SDL_SCANCODE_F,
	KEY_G = SDL_SCANCODE_G,
	KEY_H = SDL_SCANCODE_H,
	KEY_I = SDL_SCANCODE_I,
	KEY_J = SDL_SCANCODE_J,
	KEY_K = SDL_SCANCODE_K,
	KEY_L = SDL_SCANCODE_L,
	KEY_M = SDL_SCANCODE_M,
	KEY_N = SDL_SCANCODE_N,
	KEY_O = SDL_SCANCODE_O,
	KEY_P = SDL_SCANCODE_P,
	KEY_Q = SDL_SCANCODE_Q,
	KEY_R = SDL_SCANCODE_R,
	KEY_S = SDL_SCANCODE_S,
	KEY_T = SDL_SCANCODE_T,
	KEY_U = SDL_SCANCODE_U,
	KEY_V = SDL_SCANCODE_V,
	KEY_W = SDL_SCANCODE_W,
	KEY_X = SDL_SCANCODE_X,
	KEY_Y = SDL_SCANCODE_Y,
	KEY_Z = SDL_SCANCODE_Z,
	KEY_1 = SDL_SCANCODE_1,
	KEY_2 = SDL_SCANCODE_2,
	KEY_3 = SDL_SCANCODE_3,
	KEY_4 = SDL_SCANCODE_4,
	KEY_5 = SDL_SCANCODE_5,
	KEY_6 = SDL_SCANCODE_6,
	KEY_7 = SDL_SCANCODE_7,
	KEY_8 = SDL_SCANCODE_8,
	KEY_9 = SDL_SCANCODE_9,
	KEY_0 = SDL_SCANCODE_0
};


struct KeyDown
{
	unsigned code;
};

struct KeyUp
{
	unsigned code;
};

struct MouseButtonDown
{
	unsigned char button;
};

struct MouseButtonUp
{
	unsigned char button;
};

struct MouseMove
{
	int posx;
	int posy;
	int dx;
	int dy;
};

struct MouseWheel
{
	int dist;
};
} // namespace Input
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT InputComponents
{
	InputComponents(flecs::world& world);

	flecs::entity key_;
	flecs::entity keyDown_;
	flecs::entity keyUp_;

	flecs::entity mouseButtonDown_;
	flecs::entity mouseButtonUp_;
	flecs::entity mouseMove_;
	flecs::entity mouseWheel_;
};
} // namespace A3D


#endif // INPUTCOMPONENTS_H
