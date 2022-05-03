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
#include <SDL2/SDL_timer.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "Coordinator.h"
#include "IO/Log.h"
#include "Scene/SceneComponents.h"
#include "Scene/SceneSystems.h"

Coordinator::Coordinator(const InitInfo& initInfo, bool& initialized)
	: ticks_(0.0f)
	, run_(true)
{
	initialized = false;

	if (!LogInit("../Logs", LogLevel::LOG_LEVEL_TRACE))
		return;

	world_.Initialize();
	RegisterSceneSystems(*world_, *this);
	world_->set_threads(std::thread::hardware_concurrency());

	scene_.Initialize(*world_, "Scene");

	graphics_.Initialize(initInfo.graphics, initialized);
	if (!initialized)
		return;
	graphics_->InitSystems(*world_);

	initialized = true;
}

Coordinator::~Coordinator() { LogDestroy(); }

bool Coordinator::CheckErrors()
{
	const char* error = SDL_GetError();
	if (error[0])
	{
		printf(error);
		printf("\n");
	}
	return error[0] == '\0';
}

void Coordinator::BeginFrame()
{
	SDL_Event event;
	while (SDL_PollEvent(&event))
		switch (event.type)
		{
		case SDL_QUIT:
			Exit();
			break;
		}
	graphics_->BeginFrame();
}

void Coordinator::EndFrame() { graphics_->EndFrame(); }

void Coordinator::Frame()
{
	const float ticks = static_cast<float>(SDL_GetTicks()) * 0.001f;
	world_->progress(ticks - ticks_);
	ticks_ = ticks;
	UpdateScene(scene_->GetRoot());
	graphics_->DrawScene(scene_->GetRoot());
}

int Coordinator::Run()
{
	while (IsRunning() && CheckErrors())
	{
		BeginFrame();
		Frame();
		EndFrame();
	}
	return 0;
}
