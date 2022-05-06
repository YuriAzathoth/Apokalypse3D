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

#ifndef MAINTHREADSYSTEM_H
#define MAINTHREADSYSTEM_H

#include <flecs.h>
#include <utility>
#include "Apokalypse3DAPI.h"

struct MainThreadSystem
{
	flecs::system system;
};

struct MainPreUpdate{};
struct MainPostUpdate{};

using MainThreadQuery = flecs::query<MainThreadSystem>;

static inline void RegisterMainThreadSystem(flecs::world& world)
{
	world.component<MainPreUpdate>();
	world.component<MainPostUpdate>();
}

static inline MainThreadQuery CreateMainThreadQuery(flecs::world& world)
{
	return world.query<MainThreadSystem>();
}

static inline void ProgressMainThread(MainThreadQuery& query, float deltaTime = 0.0f)
{
	query.each([deltaTime](MainThreadSystem& sys)
	{
		sys.system.run_worker(0, 1, deltaTime);
	});
}

#endif // MAINTHREADSYSTEM_H
