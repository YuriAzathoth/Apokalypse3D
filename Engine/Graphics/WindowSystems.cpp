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

#include <SDL3/SDL.h>
#include "GraphicsComponents.h"
#include "IO/Log.h"
#include "WindowComponents.h"
#include "WindowSystems.h"

using namespace A3D::Components::Graphics;
using namespace A3D::Components::Window;

inline static unsigned parse_flags(WindowMode mode, bool resizeable) noexcept
{
	unsigned flags = SDL_WINDOW_ALLOW_HIGHDPI | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_INPUT_GRABBED | SDL_WINDOW_SHOWN;
	if (mode == WindowMode::Fullscreen)
		flags |= SDL_WINDOW_BORDERLESS;
	else if (mode == WindowMode::Borderless)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (resizeable)
		flags |= SDL_WINDOW_RESIZABLE;
	return flags;
}

inline static void select_hightest_resolution(uint8_t display, uint16_t& width, uint16_t& height) noexcept
{
	const int count = SDL_GetNumDisplayModes(display);
	SDL_DisplayMode mode;
	for (int i = 0; i < count; ++i)
	{
		SDL_GetDisplayMode(display, i, &mode);
		if (width < (uint16_t)mode.w || height < (uint16_t)mode.h)
		{
			width = (uint16_t)mode.w;
			height = (uint16_t)mode.h;
		}
	}
}

static void init_default(flecs::iter it,
						 size_t,
						 const Resolution* resolution,
						 const Title* title,
						 const WindowMode* mode)
{
	LogDebug("Initializing SDL video default config...");
	flecs::world w = it.world();
	w.add<Startup>();
	if (mode == nullptr)
		w.set<WindowMode>(WindowMode::Windowed);
	if (resolution == nullptr)
	{
		uint16_t width;
		uint16_t height;
		select_hightest_resolution(0, width, height);
		w.set<Resolution>({width, height});
		LogInfo("Resolution is not set: using maximum possible.");
	}
	if (title == nullptr)
		w.set<Title>({"Untitled"});
	LogDebug("Default SDL window config initialized.");
}

static void init_sdl_video(flecs::iter& it)
{
	LogDebug("Initializing SDL video...");
	flecs::world w = it.world();
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) >= 0)
	{
		LogInfo("SDL video initialized.");
		w.add<Video>();
	}
	else
	{
		LogError("Failed to initialize SDL Video: %s.", SDL_GetError());
		w.quit();
	}
}

static void init_sdl_window(flecs::iter it,
							size_t,
							const Resolution& resolution,
							const Title& title,
							const WindowMode mode)
{
	LogDebug("Initializing SDL window...");

	flecs::world w = it.world();

	SDL_Window* window = SDL_CreateWindow(title.title,
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  resolution.width,
										  resolution.height,
										  parse_flags(mode, w.has<Resizeable>()));
	if (window == nullptr)
	{
		LogError("Failed to create SDL window: %s.", SDL_GetError());
		w.quit();
		return;
	}

	/*if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
		SDL_SetWindowGrab(window, SDL_TRUE);*/

	w.set<Window>({window});
	w.remove<Startup>();

	LogInfo("SDL Window initialized.");
	LogInfo("\tWindow size: %ux%u;", resolution.width, resolution.height);
	char* modeName = strdup(w.to_json(&mode).c_str());
	modeName[strlen(modeName) - 1] = '\0';
	LogInfo("\tWindow mode: %s.", &modeName[1]);
	free(modeName);
}

static void destroy_sdl_video(flecs::entity e)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
	LogInfo("SDL video destroyed...");
}

static void destroy_sdl_window(Window& window)
{
	SDL_DestroyWindow(window.window);
	LogInfo("SDL window destroyed...");
}

namespace A3D
{
WindowSystems::WindowSystems(flecs::world& world)
{
	world.module<WindowSystems>("A3D::Systems::Window");
	world.import<GraphicsComponents>();
	world.import<WindowComponents>();

	initDefault_ = world.system<const Resolution*, const Title*, const WindowMode*>("InitDefault")
		.arg(1).singleton()
		.arg(2).singleton()
		.arg(3).singleton()
		.kind(flecs::OnStart)
		.each(init_default);

	initSdlVideo_ = world.system<>("InitSdlVideo")
		.with<Startup>().singleton()
		.with<Video>().singleton().not_()
		.kind(flecs::OnLoad)
		.iter(init_sdl_video);

	initWindow_ = world.system<const Resolution, const Title, const WindowMode>("InitWindow")
		.with<Video>().singleton()
		.with<Startup>().singleton()
		.with<Window>().singleton().not_()
		.arg(1).singleton()
		.arg(2).singleton()
		.arg(3).singleton()
		.kind(flecs::OnLoad)
		.each(init_sdl_window);

	destroyWindow_ = world.observer<Window>("DestroyWindow")
		.arg(1).singleton()
		.event(flecs::UnSet)
		.each(destroy_sdl_window);

	destroySdlVideo_ = world.observer<>("DestroySdlVideo")
		.term<const Video>().singleton()
		.event(flecs::UnSet)
		.each(destroy_sdl_video);
}

/*using Window = A3D::Components::Window::Window;

inline static bool normalize_config(WindowConfig& config) noexcept;
inline static unsigned parse_flags(const WindowConfig& config) noexcept;
inline static void select_hightest_resolution(uint8_t display, uint16_t& width, uint16_t& height) noexcept;

bool create_video(flecs::world& world)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) >= 0)
	{
		world.add<Video>();
		return true;
	}
	else
	{
		LogError("Failed to initialize SDL Video: %s.", SDL_GetError());
		world.quit();
		return false;
	}
}

bool create_window(flecs::world& world)
{
	return create_window(world, *world.get_mut<WindowConfig>());
}

bool create_window(flecs::world& world, Components::Window::WindowConfig& config)
{
	LogDebug("Initialising SDL window...");

	if (normalize_config(config))
		world.modified<WindowConfig>();

	SDL_Window* window = SDL_CreateWindow(config.title,
										  SDL_WINDOWPOS_CENTERED,
										  SDL_WINDOWPOS_CENTERED,
										  config.width,
										  config.height,
										  parse_flags(config));
	if (!window)
	{
		LogError("Failed to create SDL window: %s.", SDL_GetError());
		world.quit();
		return false;
	}

	if (SDL_SetRelativeMouseMode(SDL_TRUE) < 0)
		SDL_SetWindowGrab(window, SDL_TRUE);

	world.set<Window>({window});
	LogInfo("SDL Window has been initialized.");

	return true;
}

static void destroy_video(Video)
{
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

static void destroy_window(Window& window)
{
	if (window.window)
		SDL_DestroyWindow(window.window);
}

inline static bool normalize_config(WindowConfig& config) noexcept
{
	bool modified = false;
	if (config.width == 0 || config.height == 0)
	{
		select_hightest_resolution(0, config.width, config.height);
		modified = true;
	}
	if (config.title == nullptr)
	{
		config.title = "Apokalypse3D";
		modified = true;
	}
	return modified;
}

inline static unsigned parse_flags(const WindowConfig& config) noexcept
{
	unsigned flags = SDL_WINDOW_ALLOW_HIGHDPI;
	if (config.fullscreen)
		flags |= SDL_WINDOW_FULLSCREEN;
	if (config.borderless)
		flags |= SDL_WINDOW_BORDERLESS;
	if (config.resizeable)
		flags |= SDL_WINDOW_RESIZABLE;
	return flags;
}

inline static void select_hightest_resolution(uint8_t display, uint16_t& width, uint16_t& height) noexcept
{
	const int count = SDL_GetNumDisplayModes(display);
	SDL_DisplayMode mode;
	for (int i = 0; i < count; ++i)
	{
		SDL_GetDisplayMode(display, i, &mode);
		if (width < (uint16_t)mode.w || height < (uint16_t)mode.h)
		{
			width = (uint16_t)mode.w;
			height = (uint16_t)mode.h;
		}
	}
}

WindowSystems::WindowSystems(flecs::world& world)
{
	world.module<WindowSystems>("A3D::Systems::Window");
	world.import<GraphicsComponents>();

	unSetWindow_ = world.observer<Window>("DestroyWindow")
		.arg(1).singleton()
		.event(flecs::UnSet)
		.each(destroy_window);

	unSetVideo_ = world.observer<Video>("DestroyVideo")
		.arg(1).singleton()
		.term<Window>().singleton()
		.event(flecs::UnSet)
		.each(destroy_video);
}*/
} // namespace A3D
