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

#include "Core/EngineLog.h"
#include "Window.h"

static SDL_Window* g_window = nullptr;
static bool g_video_init = false;

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
	g_video_init = true;
	LogInfo("SDL video initialized.");
	return true;
}

void DestroyVideo()
{
	Assert(g_video_init == true, "Failed to shutdown video subsystem that has not been initialized.");
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	LogInfo("SDL video destroyed.");
}

bool CreateWindow(const char* title,
				  const WindowResolution& resolution,
				  WindowMode mode,
				  bool resizeable)
{
	Assert(g_video_init == true, "Failed to create window: video subsystem has not been initialized.");

	LogDebug("Initializing SDL window...");

	Uint32 flags = SDL_WINDOW_ALLOW_HIGHDPI;
	switch (mode)
	{
	case WindowMode::FULLSCREEN:
		flags |= SDL_WINDOW_FULLSCREEN;
		[[fallthrough]];
	case WindowMode::BORDERLESS:
		flags |= SDL_WINDOW_BORDERLESS;
		break;
	default:
		; // No flags
	}

	g_window = SDL_CreateWindow(title,
								SDL_WINDOWPOS_CENTERED,
								SDL_WINDOWPOS_CENTERED,
								resolution.width,
								resolution.height,
								flags);
	if (g_window == nullptr)
	{
		LogFatal("Failed to create SDL window: %s.", SDL_GetError());
		return false;
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
		SDL_SetWindowGrab(g_window, SDL_TRUE);

	LogInfo("SDL Window initialized.");
	LogDebug("\tResolution:\t%ux%u;", resolution.width, resolution.height);
	LogDebug("\tRefresh rate:\t%uHz;", resolution.refresh_rate);
#ifdef APOKALYPSE_LOG_DEBUG
	switch (mode)
	{
	case WindowMode::FULLSCREEN:
		LogDebug("\tMode:\t\tFullscreen.");
		break;
	case WindowMode::BORDERLESS:
		LogDebug("\tMode:\t\tBorderless.");
		break;
	case WindowMode::WINDOWED:
		LogDebug("\tMode:\t\tWindowed.");
		break;
	}
#endif // APOKALYPSE_LOG_DEBUG

	return true;
}

void DestroyWindow()
{
	Assert(g_window != nullptr, "Failed to destroy window that has not been created.");

	SDL_DestroyWindow(g_window);
	g_window = nullptr;

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

bool GetWindowPlatformData(WindowPlatformData& pd)
{
	Assert(g_window != nullptr, "Failed to get window platform data: window has not been created.");

#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(g_window, &wmi))
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
