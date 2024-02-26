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
#include "Core/EngineLog.h"
#include "SystemEvent.h"

namespace A3D
{
bool CreateSystemEventListener()
{
	LogDebug("Initializing SDL events...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		LogFatal("Failed to initialize SDL Video: %s.", SDL_GetError());
		return false;
	}
	LogInfo("SDL events initialized.");
	return true;
}

void DestroySystemEventListener()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	LogInfo("SDL events destroyed.");
}

void BindSystemEvent(SystemEventListener& listener, uint32_t event, void(*cb)(const SDL_Event&, void*), void* data)
{
	listener.callbacks.emplace(event, SystemEventCallback{cb, data});
}

uint32_t PollSystemEvents(const SystemEventListener& listener)
{
	LogTrace("SDL events processing begin...");

	decltype(listener.callbacks)::const_iterator begin, end;
	SDL_Event event;
	uint32_t count = 0;
	while (SDL_PollEvent(&event))
	{
		auto[begin, end] = listener.callbacks.equal_range(event.type);
		for (; begin != end; ++begin)
			begin->second.func(event, begin->second.data);
		++count;
	}

	LogTrace("SDL events processing end...");

	return count;
}
} // namespace A3D
