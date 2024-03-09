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

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_video.h>
#include "Core/ILog.h"
#include "SystemWindow.h"

namespace A3D
{
bool SystemWindow::s_video_initialized = false;

SystemWindow::SystemWindow(ILog* log) :
	log_(log),
	window_(nullptr)
{
}

SystemWindow::~SystemWindow()
{
	if (window_ != nullptr)
		Destroy();
}

bool SystemWindow::InitializeVideo()
{
	log_->Debug("Initializing SDL video...");
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) >= 0)
	{
		log_->Info("SDL video initialized.");
		s_video_initialized = true;
		return true;
	}
	else
	{
		log_->Fatal("Failed to initialize SDL Video: %s.", SDL_GetError());
		return false;
	}
}

void SystemWindow::ShutdownVideo()
{
	if (s_video_initialized == true)
	{
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		log_->Info("SDL video destroyed.");
	}
	else
		log_->Error("Failed to shutdown video subsystem: SDL video has not been initialized yet.");
}

bool SystemWindow::Create(const char* title, const ScreenResolution& resolution, WindowMode mode)
{
	if (s_video_initialized == false)
	{
		log_->Fatal("Failed to create system window: SDL video has not been initialized yet.");
		return false;
	}

	// If at lest one is 0...
	if (!(resolution.width && resolution.height && resolution.refresh_rate))
	{
		log_->Fatal("Failed to create system window with zero resolution.");
		return false;
	}

	log_->Debug("Initializing SDL window...");

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

	window_ = SDL_CreateWindow(title,
							   SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED,
							   resolution.width,
							   resolution.height,
							   flags);
	if (window_ == nullptr)
	{
		log_->Fatal("Failed to create SDL window: %s.", SDL_GetError());
		return false;
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
		SDL_SetWindowGrab(window_, SDL_TRUE);

	if (mode != WindowMode::WINDOWED)
	{
		SDL_DisplayMode display_mode;

		const int display_id = SDL_GetWindowDisplayIndex(window_);

		const int modes_count = SDL_GetNumDisplayModes(display_id);
		if (modes_count < 1)
		{
			log_->Fatal("Failed to get display modes for display %u: %s.", display_id, SDL_GetError());
			return false;
		}

		bool found = false;
		for (int i = 0; i < modes_count; ++i)
		{
			if (SDL_GetDisplayMode(display_id, 0, &display_mode) < 0)
			{
				log_->Fatal("Failed to get display mode %d for display %u: %s.", i, display_id, SDL_GetError());
				return false;
			}
			if (display_mode.w == resolution.width &&
				display_mode.h == resolution.height &&
				display_mode.refresh_rate == resolution.refresh_rate)
			{
				found = true;
				break;
			}
		}

		if (found == false)
		{
			log_->Fatal("Failed to set display mode %ux%u:%u: format is not supported by display %d.",
						resolution.width,
						resolution.height,
						resolution.refresh_rate,
						display_id);
			return false;
		}

		if (SDL_SetWindowDisplayMode(window_, &display_mode) < 0)
		{
			log_->Fatal("Failed to set display mode %ux%u:%u for display %d: %s.",
						resolution.width,
						resolution.height,
						resolution.refresh_rate,
						display_id,
						SDL_GetError());
			return false;
		}
	}

	log_->Info("SDL Window initialized.");

	return true;
}

void SystemWindow::Destroy()
{
	SDL_DestroyWindow(window_);
	window_ = nullptr;
	log_->Info("SDL window destroyed.");
}

bool SystemWindow::GetMaxResolution(ScreenResolution& resolution, uint8_t display_id)
{
	const int modes_count = SDL_GetNumDisplayModes(display_id);
	if (modes_count < 1)
	{
		log_->Fatal("Failed to get display modes for display %u: %s.", display_id, SDL_GetError());
		return false;
	}

	SDL_DisplayMode mode;
	if (SDL_GetDisplayMode(display_id, 0, &mode) < 0)
	{
		log_->Fatal("Failed to get maximum display mode for display %u: %s.", display_id, SDL_GetError());
		return false;
	}

	resolution.width = mode.w;
	resolution.height = mode.h;
	resolution.refresh_rate = mode.refresh_rate;

	return true;
}

bool SystemWindow::GetPlatformData(WindowPlatformData& wpd)
{
#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window_, &wmi))
	{
		log_->Fatal("Failed to get SDL Window WM info: %s.", SDL_GetError());
		return false;
	}
#endif // !BX_PLATFORM_EMSCRIPTEN

#if SDL_VIDEO_DRIVER_WINDOWS
	wpd.window = wmi.info.win.window;
#elif SDL_VIDEO_DRIVER_WINRT
	wpd.window = wmi.info.winrt.window;
#elif SDL_VIDEO_DRIVER_X11
	wpd.display = wmi.info.x11.display;
	wpd.window = reinterpret_cast<void*>(wmi.info.x11.window);
#elif SDL_VIDEO_DRIVER_COCOA
	wpd.window = wmi.info.cocoa.window;
#elif SDL_VIDEO_DRIVER_UIKIT
	wpd.window = wmi.info.uikit.window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	wpd.display = wmi.info.wl.display;
	wpd.window = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_WAYLAND
	wpd.display = wmi.info.wl.display;
	wpd.window = wmi.info.wl.egl_window;
#elif SDL_VIDEO_DRIVER_ANDROID
	wpd.window = wmi.info.android.window;
#elif SDL_VIDEO_DRIVER_EMSCRIPTEN
	wpd.window = (void*)"#canvas";
#endif // BX_PLATFORM_

	return true;
}
} // namespace A3D
