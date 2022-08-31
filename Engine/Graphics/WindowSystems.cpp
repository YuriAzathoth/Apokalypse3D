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

#include <SDL2/SDL.h>
#include "IO/Log.h"
#include "WindowComponents.h"
#include "WindowSystems.h"

using namespace A3D::Components::Window;

namespace A3D
{
using Window = A3D::Components::Window::Window;

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
	if (window != nullptr)
	{
		LogInfo("SDL Window has been initialized.");
		world.set<Window>({window});
		return true;
	}
	else
	{
		LogError("Failed to create SDL window: %s", SDL_GetError());
		world.quit();
		return false;
	}
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
	flecs::_::cpp_type<WindowSystems>::id_explicit(world, 0, false);
	world.module<WindowSystems>("A3D::Systems::Window");

	unSetWindow_ = world.observer<Window>("DestroyWindow")
		.arg(1).singleton()
		.event(flecs::UnSet)
		.each(destroy_window);

	unSetVideo_ = world.observer<Video>("DestroyVideo")
		.arg(1).singleton()
		.term<Window>().singleton()
		.event(flecs::UnSet)
		.each(destroy_video);
}
} // namespace A3D
