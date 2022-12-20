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

#include <stdlib.h>
#include <string.h>
#include <thread>
#include "Engine.h"
#include "IO/Log.h"

namespace A3D
{
Engine::Engine(const EngineInfo& init) :
	app(world)
{
	LogDebug("Initializing Core...");

	unsigned threads_num;
	if (init.threads == 0)
		threads_num = std::thread::hardware_concurrency();
	else
		threads_num = init.threads;

	/*if (argc > 1)
	{
		char* argw;
		char** argend = argv + argc;
		for (char** arg = argv + 1; arg < argend; ++arg)
			if (*arg[0] == '-')
			{
				argw = *arg + 1;
				LogDebug("Command line parameter: %s", argw);
				if (!strcmp(argw, "display"))
				{
					if (arg + 1 < argend)
					{
						int display = atoi(arg[1]);
						world.set<WindowDisplay>({display});
					}
				}
				else if (!strcmp(argw, "fullscreen"))
					world.add<WindowFullscreen>();
				else if (!strcmp(argw, "size"))
				{
					if (arg + 2 < argend)
					{
						int width = atoi(arg[1]);
						int height = atoi(arg[2]);
						world.set<WindowSize>({width, height});
					}
				}
				else if (!strcmp(argw, "vsync"))
					world.add<WindowVSync>();
				else if (!strcmp(argw, "vk"))
					world.set<GraphicsRenderer>({bgfx::RendererType::Vulkan});
				else if (!strcmp(argw, "gl"))
					world.set<GraphicsRenderer>({bgfx::RendererType::OpenGL});
				else if (!strcmp(argw, "gles"))
					world.set<GraphicsRenderer>({bgfx::RendererType::OpenGLES});
#if defined(_WIN32)
				else if (!strcmp(argw, "dx9"))
					world.set<GraphicsRenderer>({bgfx::RendererType::Direct3D9});
				else if (!strcmp(argw, "dx11"))
					world.set<GraphicsRenderer>({bgfx::RendererType::Direct3D11});
				else if (!strcmp(argw, "dx12"))
					world.set<GraphicsRenderer>({bgfx::RendererType::Direct3D12});
#elif defined(__OSX__)
				else if (!strcmp(argw, "metal"))
					world.set<GraphicsRenderer>({bgfx::RendererType::Metal});
#endif
				else if (!strcmp(argw, "threads"))
				{
					if (arg + 1 < argend)
						numThreads = (unsigned)atoi(*++arg);
				}
			}
	}*/

	app.threads(threads_num);

	LogInfo("Core has been initialized.");
}

int Engine::Run()
{
	return world.app().run();
}
} // namespace A3D
