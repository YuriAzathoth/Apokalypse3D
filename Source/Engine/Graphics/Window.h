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

#ifndef GRAPHICS_WINDOW_H
#define GRAPHICS_WINDOW_H

#include <stdint.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
enum WindowMode : uint8_t
{
	FULLSCREEN,
	BORDERLESS,
	WINDOWED
};

struct WindowPlatformData
{
	void* window;
	void* display;
};

struct WindowResolution
{
	uint16_t width;
	uint16_t height;
	uint16_t refresh_rate;
};

APOKALYPSE3DAPI_EXPORT bool CreateVideo();
APOKALYPSE3DAPI_EXPORT void DestroyVideo();

APOKALYPSE3DAPI_EXPORT bool CreateWindow(const char* title,
										 const WindowResolution& resolution,
										 WindowMode mode,
										 bool resizeable);

APOKALYPSE3DAPI_EXPORT void DestroyWindow();

APOKALYPSE3DAPI_EXPORT bool GetMaxWindowResolution(WindowResolution& resolution, uint8_t display_id);
APOKALYPSE3DAPI_EXPORT bool GetWindowPlatformData(WindowPlatformData& pd);
} // namespace A3D

#endif // GRAPHICS_WINDOW_H
