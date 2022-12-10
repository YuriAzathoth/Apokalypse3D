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

#include <bgfx/bgfx.h>
#include "build_config/SDL_build_config.h"
#include "GraphicsPlatform.h"

#if SDL_VIDEO_DRIVER_WINDOWS // Windows
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define SDL_ENABLE_SYSWM_WINDOWS
#elif SDL_VIDEO_DRIVER_WINRT // X Window System
#include <inspectable.h>
#define SDL_ENABLE_SYSWM_WINRT
#elif SDL_VIDEO_DRIVER_X11 // X Window System
#include <X11/Xlib.h>
#define SDL_ENABLE_SYSWM_X11
#elif SDL_VIDEO_DRIVER_COCOA // Apple Mac OS X
// TODO: Add propper includes
#define SDL_ENABLE_SYSWM_COCOA
#elif SDL_VIDEO_DRIVER_UIKIT // Apple iOS
// TODO: Add propper includes
#define SDL_ENABLE_SYSWM_UIKIT
#elif SDL_VIDEO_DRIVER_WAYLAND // Wayland
// TODO: Add propper includes
#include <wayland-egl.h>
#define SDL_ENABLE_SYSWM_WAYLAND
#elif SDL_VIDEO_DRIVER_ANDROID // Android
#include <EGL/eglplatform.h>
#include <android/native_window_jni.h>
#define SDL_ENABLE_SYSWM_ANDROID
#elif SDL_VIDEO_DRIVER_VIVANTE // Vivante
// TODO: Add propper includes
#define SDL_ENABLE_SYSWM_VIVANTE
#elif SDL_VIDEO_DRIVER_EMSCRIPTEN // Emscripten
//define nothing
#else // SDL_VIDEO_DRIVER_
#error Unsupported SysWM platform
#endif // SDL_VIDEO_DRIVER_
#include <SDL3/SDL_syswm.h>

bool bind_bgfx_to_sdl(bgfx::PlatformData& pd, SDL_Window* window) noexcept
{
#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	if (SDL_GetWindowWMInfo(window, &wmi, SDL_SYSWM_CURRENT_VERSION) < 0)
		return false;
#endif // !BX_PLATFORM_EMSCRIPTEN

#if SDL_VIDEO_DRIVER_WINDOWS
	pd.nwh = wmi.info.win.window;
#elif SDL_VIDEO_DRIVER_WINRT
	pd.nwh = wmi.info.winrt.window;
#elif SDL_VIDEO_DRIVER_X11
	pd.ndt = wmi.info.x11.display;
	pd.nwh = reinterpret_cast<void*>(wmi.info.x11.window);
#elif SDL_VIDEO_DRIVER_COCOA
	pd.nwh = wmi.info.cocoa.window;
#elif SDL_VIDEO_DRIVER_UIKIT
	pd.nwh = wmi.info.uikit.window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	pd.ndt = wmi.info.wl.display;
	pd.nwh = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	pd.ndt = wmi.info.wl.display;
	pd.nwh = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_ANDROID
	pd.nwh = wmi.info.android.window;
#elif SDL_VIDEO_DRIVER_EMSCRIPTEN
	pd.nwh = (void*)"#canvas";
#endif // BX_PLATFORM_

	return true;
}
