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

Coordinator::Coordinator()
	: run_(true)
{
}

bool Coordinator::Start()
{
	const char* windowTitle = "Hello World!";
	const int windowWidth = 800;
	const int windowHeight = 600;
	const int glMajor = 3;
	const int glMinor = 3;
	const int vsyns = 0;

	if (!graphics_.Start(windowTitle, windowWidth, windowHeight, glMajor, glMinor, vsyns))
	{
		printf("ERROR: %s\n", graphics_.GetErrorString());
		return false;
	}

	SDL_Event event;
	while (run_)
	{
		while (SDL_PollEvent(&event))
			switch (event.type)
			{
			case SDL_QUIT:
				run_ = false;
				break;
			}
		graphics_.BeginFrame();
		graphics_.EndFrame();
		while (graphics_.PollErrors())
		{
			printf("ERROR: %s\n", graphics_.GetErrorString());
			run_ = false;
		}
	}

	return true;
}

void Coordinator::Stop() { run_ = false; }
