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

#ifndef ASYNCTASKSYSTEMS_H
#define ASYNCTASKSYSTEMS_H

#include <flecs.h>
#include <functional>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Async
{
using TaskFunc = std::function<bool(const flecs::world&, flecs::world&)>;
}
}
}

APOKALYPSE3DAPI_EXPORT void a3d_async_planner_set(flecs::world& world, unsigned short threads);
APOKALYPSE3DAPI_EXPORT bool a3d_async_planner_is_done(flecs::world& world);
APOKALYPSE3DAPI_EXPORT bool a3d_async_task_add(flecs::world& world, A3D::Components::Async::TaskFunc task_func);

struct APOKALYPSE3DAPI_EXPORT AsyncTaskSystems
{
	AsyncTaskSystems(flecs::world& world);

	flecs::entity createPlanner_;
	flecs::entity onSetThreadsCount_;

	flecs::entity worldsMerge_;
};

#endif // ASYNCTASKSYSTEMS_H
