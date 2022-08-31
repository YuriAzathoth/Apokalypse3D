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

#ifndef WINDOWCOMPONENTS_H
#define WINDOWCOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

struct SDL_Window;

namespace A3D
{
namespace Components
{
namespace Window
{
struct Video {};

struct Window
{
	SDL_Window* window;
};

struct WindowConfig
{
	const char* title;
	uint16_t width;
	uint16_t height;
	bool fullscreen;
	bool borderless;
	bool resizeable;
};
} // namespace Window
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT WindowComponents
{
	WindowComponents(flecs::world& world);

	flecs::entity video_;
	flecs::entity window_;
	flecs::entity windowConfig_;
};
} // namespace A3D

#endif // WINDOWCOMPONENTS_H
