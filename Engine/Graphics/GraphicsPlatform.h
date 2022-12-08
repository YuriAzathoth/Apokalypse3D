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

#ifndef GRAPHICS_PLATFORM_H
#define GRAPHICS_PLATFORM_H

struct SDL_SysWMinfo;
struct SDL_Window;
namespace bgfx { struct PlatformData; }

bool bind_bgfx_to_sdl(bgfx::PlatformData& pd, SDL_Window* window) noexcept;

#endif // GRAPHICS_PLATFORM_H
