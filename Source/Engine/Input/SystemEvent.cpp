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
#include "Core/ILog.h"
#include "SystemEvent.h"

namespace A3D
{
SystemEventListener::SystemEventListener(ILog* log) :
	log_(log),
	initialized_(false)
{
}

SystemEventListener::~SystemEventListener()
{
	if (initialized_)
		Shutdown();
}

bool SystemEventListener::Initialize()
{
	log_->Debug("Initializing SDL events...");
	if (SDL_InitSubSystem(SDL_INIT_EVENTS) < 0)
	{
		log_->Fatal("Failed to initialize SDL Video: %s.", SDL_GetError());
		return false;
	}
	initialized_ = true;
	log_->Info("SDL events initialized.");
	return true;
}

void SystemEventListener::Shutdown()
{
	SDL_QuitSubSystem(SDL_INIT_EVENTS);
	initialized_ = false;
	log_->Info("SDL events destroyed.");
}

bool SystemEventListener::PollEvents()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
			return false;
	}
	return true;
}
} // namespace A3D
