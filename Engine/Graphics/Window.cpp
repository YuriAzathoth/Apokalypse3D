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

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_video.h>

#ifdef __WIN32__
#ifdef CreateWindow
#undef CreateWindow
#endif // CreateWindow
#ifdef ERROR
#undef ERROR
#endif // ERROR
#endif // __WIN32__

#include "IO/Log.h"
#include "Window.h"

namespace A3D
{
bool CreateVideo()
{
	LogDebug("Initializing SDL video...");
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		LogFatal("Failed to initialize SDL Video: %s.", SDL_GetError());
		return false;
	}
	LogInfo("SDL video initialized.");
	return true;
}

void DestroyVideo()
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	LogInfo("SDL video destroyed.");
}

bool CreateWindow(Window& window,
				  const char* title,
				  const WindowResolution& resolution,
				  WindowMode mode,
				  bool resizeable)
{
	LogDebug("Initializing SDL window...");

	Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
	switch (mode)
	{
	case WindowMode::FULLSCREEN:
		flags |= SDL_WINDOW_FULLSCREEN;
		[[fallthrough]];
	case WindowMode::BORDERLESS:
		flags |= SDL_WINDOW_BORDERLESS;
	default:
		; // No flags
	}

	window.window = SDL_CreateWindow(title,
									 SDL_WINDOWPOS_CENTERED,
									 SDL_WINDOWPOS_CENTERED,
									 resolution.width,
									 resolution.height,
									 flags);
	if (window.window == nullptr)
	{
		LogFatal("Failed to create SDL window: %s.", SDL_GetError());
		return false;
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
		SDL_SetWindowGrab(window.window, SDL_TRUE);

	LogInfo("SDL Window initialized.");
	LogDebug("\tResolution: %ux%u;", resolution.width, resolution.height);
	LogDebug("\tRefresh rate: %uHz;", resolution.refresh_rate);
	LogDebugBody(
	{
		switch (mode)
		{
		case WindowMode::FULLSCREEN:
			LogDebug("\tMode: Fullscreen.");
			break;
		case WindowMode::BORDERLESS:
			LogDebug("\tMode: Borderless.");
			break;
		case WindowMode::WINDOWED:
			LogDebug("\tMode: Windowed.");
			break;
		}
	});

	return true;
}

void DestroyWindow(Window& window)
{
	SDL_DestroyWindow(window.window);
	LogInfo("SDL window destroyed.");
}

bool GetMaxWindowResolution(WindowResolution& resolution, uint8_t display_id)
{
	const int modes_count = SDL_GetNumDisplayModes(display_id);
	if (modes_count < 1)
	{
		LogFatal("Failed to get display modes for display %u: %s.", display_id, SDL_GetError());
		return false;
	}
	SDL_DisplayMode mode;
	if (SDL_GetDisplayMode(display_id, 0, &mode) < 0)
	{
		LogFatal("Failed to get maximum display mode for display %u: %s.", display_id, SDL_GetError());
		return false;
	}
	resolution.width = mode.w;
	resolution.height = mode.h;
	resolution.refresh_rate = mode.refresh_rate;
	return true;
}

bool GetWindowPlatformData(WindowPlatformData& pd, const Window& window)
{
#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window.window, &wmi))
	{
		LogFatal("Failed to get SDL Window WM info: %s.", SDL_GetError());
		return false;
	}
#endif // !BX_PLATFORM_EMSCRIPTEN

#if SDL_VIDEO_DRIVER_WINDOWS
	pd.window = wmi.info.win.window;
#elif SDL_VIDEO_DRIVER_WINRT
	pd.window = wmi.info.winrt.window;
#elif SDL_VIDEO_DRIVER_X11
	pd.display = wmi.info.x11.display;
	pd.window = reinterpret_cast<void*>(wmi.info.x11.window);
#elif SDL_VIDEO_DRIVER_COCOA
	pd.window = wmi.info.cocoa.window;
#elif SDL_VIDEO_DRIVER_UIKIT
	pd.window = wmi.info.uikit.window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	pd.display = wmi.info.wl.display;
	pd.window = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	pd.display = wmi.info.wl.display;
	pd.window = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_ANDROID
	pd.window = wmi.info.android.window;
#elif SDL_VIDEO_DRIVER_EMSCRIPTEN
	pd.window = (void*)"#canvas";
#endif // BX_PLATFORM_

	return true;
}
} // namespace A3D
