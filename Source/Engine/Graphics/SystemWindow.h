/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2024 Yuriy Zinchenko

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

#ifndef GRAPHICS_SYSTEM_WINDOW_H
#define GRAPHICS_SYSTEM_WINDOW_H

#include <stdint.h>
#include "Common/Config.h"
#include "EngineAPI.h"
#include "GraphicsConfig.h"

struct SDL_Window;

namespace A3D
{
class ILog;

class ENGINEAPI_EXPORT SystemWindow
{
public:
	explicit SystemWindow(ILog* log);
	~SystemWindow();

	bool InitializeVideo();
	void ShutdownVideo();

	bool Create(const char* title, const ScreenResolution& resolution, WindowMode mode);
	void Destroy();

	bool GetMaxResolution(ScreenResolution& resolution, uint8_t display_id);

	bool GetPlatformData(WindowPlatformData& wpd);

private:
	ILog* log_;
	SDL_Window* window_;

public:
	static bool IsVideoInitialized() { return s_video_initialized; }

private:
	static bool s_video_initialized;
};

/*enum WindowMode : uint8_t
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

ENGINEAPI_EXPORT bool CreateVideo();
ENGINEAPI_EXPORT void DestroyVideo();

ENGINEAPI_EXPORT bool CreateWindow(const char* title,
										 const WindowResolution& resolution,
										 WindowMode mode,
										 bool resizeable);

ENGINEAPI_EXPORT void DestroyWindow();

ENGINEAPI_EXPORT bool GetMaxWindowResolution(WindowResolution& resolution, uint8_t display_id);
ENGINEAPI_EXPORT bool GetWindowPlatformData(WindowPlatformData& pd);*/
} // namespace A3D

#endif // GRAPHICS_SYSTEM_WINDOW_H
