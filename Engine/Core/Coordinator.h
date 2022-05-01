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

#ifndef COORDINATOR_H
#define COORDINATOR_H

#include <memory>
#include <flecs.h>
#include "Apokalypse3DAPI.h"
#include "Container/LazyInit.h"
#include "Graphics/Graphics.h"
#include "Scene/Scene.h"

class APOKALYPSE3DAPI_EXPORT Coordinator
{
public:
	struct InitInfo
	{
		Graphics::InitInfo graphics;
	};

	Coordinator(const InitInfo& initInfo, bool& initialized);
	~Coordinator();

	int Run();
	bool CheckErrors();
	void BeginFrame();
	void EndFrame();
	void Frame();

	void Exit() noexcept { run_ = false; }

	Graphics& GetGraphics() noexcept { return *graphics_; }
	const Graphics& GetGraphics() const noexcept { return *graphics_; }
	Scene& GetScene() noexcept { return *scene_; }
	const Scene& GetScene() const noexcept { return *scene_; }
	flecs::world& GetWorld() noexcept { return *world_; }
	const flecs::world& GetWorld() const noexcept { return *world_; }
	bool IsRunning() const noexcept { return run_; }

private:
	LazyInit<Graphics> graphics_;
	LazyInit<Scene> scene_;
	LazyInit<flecs::world> world_;
	float ticks_;
	bool run_;
};

#endif // COORDINATOR_H
