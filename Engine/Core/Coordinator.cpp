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

#include <SDL2/SDL_events.h>
#include <stdio.h>
#include <stdlib.h>
#include "Coordinator.h"

Coordinator::Coordinator(const InitInfo& initInfo, bool& initialized)
	: event_(nullptr)
	, run_(true)
{
	initialized = false;

	graphics_.Initialize(initInfo.graphics, initialized);
	if (!initialized)
	{
		printf("ERROR: %s\n", SDL_GetError());
		return;
	}

	event_ = new SDL_Event;

	initialized = true;
}

Coordinator::~Coordinator() { delete reinterpret_cast<SDL_Event*>(event_); }

void Coordinator::BeginFrame()
{
	SDL_Event* event = reinterpret_cast<SDL_Event*>(event_);
	while (SDL_PollEvent(event))
		switch (event->type)
		{
		case SDL_QUIT:
			run_ = false;
			break;
		}
	graphics_->BeginFrame();
}

void Coordinator::EndFrame() { graphics_->EndFrame(); }
void Coordinator::Draw() {}

int Coordinator::Run()
{
	while (run_)
	{
		BeginFrame();
		Draw();
		EndFrame();
	}
	return 0;
}

void Coordinator::Exit() { run_ = false; }
