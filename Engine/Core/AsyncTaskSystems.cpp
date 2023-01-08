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
#include "AsyncTaskSystems.h"
#include "IO/Log.h"
#include <new>

using namespace A3D::Components::Async;

static int thread_process(void* param)
{
	ThreadContext* context = reinterpret_cast<ThreadContext*>(param);

	Assert(param != NULL, "Async task planner thread context is null.");
	Assert(context->world, "Async task world is null.");

	// Wait till all planner threads are created
	while (atomic_load(&context->state) == THREAD_STATE_WAIT_TO_RUN)
		;

	Task task;
	do
	{
		// Wait for merging thread's async stage in main world
		while (atomic_load(&context->state) == THREAD_STATE_READY_TO_MERGE)
			a3d_sleep(0, 100);

		if (a3d_async_queue_pop(const_cast<a3d_async_queue_t*>(context->tasks_all), &task))
		{
			atomic_store(&context->state, THREAD_STATE_PROCESSING);
			if (task.run(*context->world, context->stage))
			{
				atomic_store(&context->state, THREAD_STATE_READY_TO_MERGE);
				// LogTrace("Async task done.");
			}
			else
				return 1;
		}
		else
			a3d_sleep(0, 100);
	} while (atomic_load(&context->state) != THREAD_STATE_SHUTDOWNING);
	return 0;
}

void a3d_async_planner_set(flecs::world& world, unsigned short threads)
{
	world.entity().set<SetThreads>({threads});
}

bool a3d_async_planner_is_done(flecs::world& world)
{
	Assert(world.has<PlannerStatus>(), "PlannerStatus singleton has not been created yet.");
	return world.get<PlannerStatus>()->tasks_waiting > 0;
}

bool a3d_async_task_add(flecs::world& world, A3D::Components::Async::TaskFunc task_func)
{
	Assert(world.has<Planner>(), "Planner singleton has not been created yet.");
	Assert(world.has<PlannerStatus>(), "PlannerStatus singleton has not been created yet.");
	Planner* planner = const_cast<Planner*>(world.get<Planner>());
	Task task{task_func};
	a3d_async_queue_push(planner->tasks, &task);
	++world.get_mut<PlannerStatus>()->tasks_waiting;
	return true;
}

static void create_planner(flecs::entity e, const SetThreads& count)
{
	// LogTrace("Creating async task planner with threads %u...", count.threads);
	flecs::world w = e.world();
	w.set<Planner>({});
	w.set<PlannerStatus>({0, 0});
}

static void set_threads_count(flecs::entity e, Planner& planner, const SetThreads& count)
{
	// Each time changing threads count we will reallocate buffer
	// Because threads count changing is rare operation
	if (planner.threads_count > 0)
		a3d_async_planner_destroy(planner);

	// LogTrace("Set async task planner threads to %u.", count.threads);

	if (count.threads > 0)
	{
		// LogTrace("Creating %u threads...", count.threads);

		const size_t threads_size = count.threads * sizeof(Thread);

		planner.bufmem = (unsigned char*)malloc(sizeof(a3d_async_queue_t) +
												threads_size +
												count.threads * sizeof(ThreadContext));
		if (planner.bufmem != nullptr)
		{
			uint8_t* buffptr = planner.bufmem;

			// Create tasks async queue in contiguous memory
			planner.tasks = reinterpret_cast<a3d_async_queue_t*>(buffptr);
			a3d_async_queue_new(planner.tasks, sizeof(Task));
			a3d_async_queue_reserve(planner.tasks, 32);
			buffptr += sizeof(a3d_async_queue_t);

			// Create and fill threads and their contexts in contiguous memory
			planner.threads = reinterpret_cast<Thread*>(buffptr);
			buffptr += threads_size;

			Thread* thread = planner.threads;
			ThreadContext* context = reinterpret_cast<ThreadContext*>(buffptr);
			flecs::world w = e.world();
			for (unsigned i = 0; i < count.threads; ++i)
			{
				new (&context->stage) flecs::world(w.async_stage());
				context->world = &w;
				context->tasks_all = planner.tasks;
				context->thread_id = i;

				atomic_store(&context->progress, 0);
				atomic_store(&context->whole, 0);
				atomic_store(&context->state, THREAD_STATE_WAIT_TO_RUN);

				thread->context = context;
				thread->thread = a3d_thread_create(thread_process, context);
				if (thread->thread == 0)
				{
					LogWarning("Failed to create %u'th async loader thread: thread creation error.", i);
					planner.threads_count = i;
					break;
				}

				++context;
				++thread;
			}
		}
		else
		{
			LogError("Failed to create async loader threads: out of memory.");
			e.world().remove<Planner>();
			return;
		}
	}
	else // (count.threads == 0)
	{
		planner.tasks = nullptr;
		planner.threads = nullptr;
		planner.bufmem = nullptr;
	}

	planner.threads_count = count.threads;

	// Launch created threads
	for (unsigned i = 0; i < planner.threads_count; ++i)
		atomic_store(&planner.threads[i].context->state, THREAD_STATE_IDLE);

	e.remove<SetThreads>();
}

static void merge_worlds(flecs::iter& it, size_t, Planner& planner)
{
	flecs::world w = it.world();
	ThreadContext* context;
	unsigned done = 0;
	w.defer_suspend();
	for (unsigned i = 0; i < planner.threads_count; ++i)
	{
		context = planner.threads[i].context;
		if (atomic_load(&context->state) == THREAD_STATE_READY_TO_MERGE)
		{
			context->stage.merge();
			atomic_store(&context->state, THREAD_STATE_IDLE);
			++done;
		}
	}
	// LogTrace("Merged %u async task threads' world.", done);
	w.defer_resume();
	PlannerStatus* status = w.get_mut<PlannerStatus>();
	status->tasks_done += done;
	status->tasks_waiting -= done;
}

AsyncTaskSystems::AsyncTaskSystems(flecs::world& world)
{
	world.import<AsyncTaskComponents>();
	world.module<AsyncTaskSystems>("A3D::Systems::Async");

	createPlanner_ = world.observer<const SetThreads>("CreatePlanner")
					 .term<const Planner>().singleton().not_()
					 .event(flecs::OnAdd)
					 .each(create_planner);

	onSetThreadsCount_ = world.observer<Planner, const SetThreads>("SetThreadsCount")
						 .arg(1).singleton()
						 .event(flecs::OnSet)
						 .each(set_threads_count);

	worldsMerge_ = world.system<Planner>("MergeWorlds")
				   .arg(1).singleton()
				   .kind(flecs::OnStore)
				   .no_readonly()
				   .each(merge_worlds);
}
