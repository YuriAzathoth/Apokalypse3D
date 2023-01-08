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

#include "AsyncTaskComponents.h"
#include "IO/Log.h"

using namespace A3D::Components::Async;

void A3D::Components::Async::a3d_async_planner_destroy(Planner& planner)
{
//	LogDebug("Destroy async task planner.");
	if (planner.threads_count)
	{
//		LogDebug("Destroying %u threads...", planner.threads_count);
		for (Thread* thread = planner.threads; thread < planner.threads + planner.threads_count; ++thread)
		{
			thread->context->state.store(THREAD_STATE_SHUTDOWNING);
			a3d_thread_destroy(thread->thread);
		}
		for (Thread* thread = planner.threads; thread < planner.threads + planner.threads_count; ++thread)
			thread->context->stage.~world();
		a3d_async_queue_free(planner.tasks);
		free(planner.bufmem);
	}
}

static void destroy_planner(Planner& planner)
{
	a3d_async_planner_destroy(planner);
}

AsyncTaskComponents::AsyncTaskComponents(flecs::world& world)
{
	world.module<AsyncTaskComponents>("A3D::Components::Async");

	planner_ = world.component<Planner>();
	plannerStatus_ = world.component<PlannerStatus>().member<unsigned>("tasks_waiting").member<unsigned>("tasks_done");
	setThreads_ = world.component<SetThreads>().member<unsigned short>("threads");

	onRemovePlanner_ = world.observer<Planner>("DestroyPlanner")
		.event(flecs::UnSet)
		.each(destroy_planner);
}
