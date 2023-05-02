/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2023 Yuriy Zinchenko

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

#include <SDL_events.h>
#include "Graphics/Window.h"
#include "Graphics/Renderer.h"
#include "System/SystemEventListener.h"

using namespace A3D;

static void OnQuit(const SDL_Event& event, void* run)
{
	*reinterpret_cast<bool*>(run) = false;
}

int main()
{
	bool run = true;

	SystemEventListener listener;
	CreateSystemEventListener();
	BindSystemEvent(listener, SDL_QUIT, OnQuit, &run);

	CreateVideo();

	WindowResolution wres{800, 600};
	CreateWindow("Hello, World!", wres, WindowMode::WINDOWED, false);

	WindowPlatformData wpd;
	GetWindowPlatformData(wpd);

	RendererGpu gpu{};
	RendererResolution rres{wres.width, wres.height};
	CreateRenderer(gpu, wpd.window, wpd.display, rres, RendererType::OpenGL, 16, 0, false, false);

	const uint8_t threads_count = 4;
	RendererThreadContext* contexts = CreateRendererThreadContexts(threads_count);

	while (run)
	{
		PollSystemEvents(listener);
		BeginRendererFrame(rres);
		BeginRendererThreadContextsFrame(contexts, threads_count);
		EndRendererThreadContextsFrame(contexts, threads_count);
		EndRendererFrame();
	}

	DestroyRendererThreadContexts(contexts);
	DestroyRenderer();
	DestroyWindow();
	DestroyVideo();
	DestroySystemEventListener();

	return 0;
}
