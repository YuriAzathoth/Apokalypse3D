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
#include <flecs.h>
#include <stdio.h>
#include <stdlib.h>
#include <thread>
#include "Coordinator.h"
#include "Scene/SceneComponents.h"
#include "Scene/SceneSystems.h"

Coordinator::Coordinator(const InitInfo& initInfo, bool& initialized)
	: ticks_(0.0f)
	, run_(true)
{
	initialized = false;

	world_.Initialize();
	RegisterSceneSystems(*world_);
	world_->set_threads(std::thread::hardware_concurrency());
	scene_ = world_->entity<Scene>().add<Node>().add<Translation>();

	graphics_.Initialize(initInfo.graphics, initialized);
	if (!initialized)
	{
		printf("ERROR: %s\n", SDL_GetError());
		return;
	}

	initialized = true;
}

Coordinator::~Coordinator() {}

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
	UpdateScene(scene_);
	graphics_->DrawScene(scene_);
}

int Coordinator::Run()
{
	while (IsRunning())
	{
		BeginFrame();
		Frame();
		EndFrame();
	}
	return 0;
}
