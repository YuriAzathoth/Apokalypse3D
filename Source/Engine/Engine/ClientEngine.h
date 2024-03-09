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

#ifndef ENGINE_CLIENT_ENGINE_H
#define ENGINE_CLIENT_ENGINE_H

#include "EngineAPI.h"
#include "Graphics/Renderer.h"
#include "Graphics/RendererAllocator.h"
#include "Graphics/RendererCallback.h"
#include "Graphics/SystemWindow.h"
#include "Input/SystemEvent.h"

namespace A3D
{
class GraphicsConfig;
class IAllocator;
class ILog;

class ENGINEAPI_EXPORT ClientEngine
{
public:
	ClientEngine(IAllocator* alloc, ILog* log);
	~ClientEngine();

	bool Initialize(const char* window_title, GraphicsConfig& graphics_config);
	void Shutdown();

	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	RendererAllocator ralloc_;
	RendererCallback rcallback_;
	Renderer renderer_;
	SystemWindow window_;
	SystemEventListener system_event_;
	IAllocator* alloc_;
	ILog* log_;
};
} // namespace A3D

#endif // ENGINE_CLIENT_ENGINE_H
