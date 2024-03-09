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

#ifndef ENGINE_APPLICATION_H
#define ENGINE_APPLICATION_H

#include "Core/DefaultAllocator.h"
#include "Core/DefaultLog.h"
#include "EngineAPI.h"
#include "PluginStorage.h"

namespace A3D
{
class ClientEngine;
class ServerEngine;

class ENGINEAPI_EXPORT Application
{
public:
	Application();
	~Application();

	int Run(const char* window_title, int argc, char** argv);

	bool LoadMainPlugin(const char* filename);

private:
	enum StartupFlags : uint8_t
	{
		FLAG_HEADLESS = 0x1
	};

	struct StartupOptions
	{
		uint8_t flags;
	};

	bool ParseArgs(StartupOptions& options, int argc, char** argv);
	bool Initialize(const char* window_title, const StartupOptions& options);
	void Shutdown();
	bool MainLoop();

	DefaultAllocator alloc_;
	DefaultLog log_;
	PluginStorage plugins_;
	ServerEngine* server_;
	ClientEngine* client_;
};
} // namespace A3D

#endif // ENGINE_APPLICATION_H
