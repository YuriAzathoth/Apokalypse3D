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

#include "ClientEngine.h"

namespace A3D
{
ClientEngine::ClientEngine(IAllocator* alloc, ILog* log) :
	ralloc_(alloc),
	rcallback_(log),
	renderer_(log, &ralloc_, &rcallback_),
	window_(log),
	system_event_(log),
	alloc_(alloc),
	log_(log)
{
}

ClientEngine::~ClientEngine()
{
}

bool ClientEngine::Initialize(const char* window_title, GraphicsConfig& graphics_config)
{
	if (!system_event_.Initialize())
		return false;

	if (!window_.InitializeVideo())
		return false;

	if ((graphics_config.resolution.width | graphics_config.resolution.height) == 0)
		if (!window_.GetMaxResolution(graphics_config.resolution, 0))
			return false;

	if (!window_.Create(window_title, graphics_config.resolution, graphics_config.window_mode))
		return false;

	WindowPlatformData wpd;
	if (!window_.GetPlatformData(wpd))
		return false;

	if (!renderer_.Initialize(graphics_config, wpd))
		return false;

	return true;
}

void ClientEngine::Shutdown()
{
	renderer_.Shutdown();
	window_.Destroy();
	window_.ShutdownVideo();
	system_event_.Shutdown();
}

bool ClientEngine::PreUpdate()
{
	if (!system_event_.PollEvents())
		return false;

	return true;
}

bool ClientEngine::Update()
{
	return true;
}

bool ClientEngine::PostUpdate()
{
	return true;
}
} // namespace A3D
