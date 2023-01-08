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

#ifndef ASYNCTASKCOMPONENTS_H
#define ASYNCTASKCOMPONENTS_H

#include <atomic>
#include <bx/thread.h>
#include <flecs.h>
#include <functional>
#include "Apokalypse3DAPI.h"
#include "Container/async_queue.h"
#include "Container/vector.h"

namespace A3D
{
namespace Components
{
namespace Async
{
struct ThreadContext;

using TaskFunc = std::function<bool(const flecs::world&, flecs::world&)>;

enum : uint8_t
{
	/// Thread is just created and waits till Planner accepted as world singleton
	THREAD_STATE_WAIT_TO_RUN,

	/// Thread is idle (not processing tasks at the moment)
	THREAD_STATE_IDLE,

	/// Thread does one task
	THREAD_STATE_PROCESSING,

	/// Thread is ready to merge
	THREAD_STATE_READY_TO_MERGE,

	/// Thread is made to shutdown (stops thread inner loop)
	THREAD_STATE_SHUTDOWNING
};

struct Task
{
	TaskFunc run;
};

struct Thread
{
	ThreadContext* context;
	a3d_thread_t thread;
};

struct ThreadContext
{
	Task task;
	flecs::world stage;
	const flecs::world* world;
	const a3d_async_queue_t* tasks_all;
	unsigned thread_id;
	std::atomic<unsigned> progress;
	std::atomic<unsigned> whole;
	std::atomic<unsigned> state;
};

struct Planner
{
	a3d_async_queue_t* tasks;
	Thread* threads;
	unsigned char* bufmem;
	unsigned threads_count;
};

struct PlannerStatus
{
	unsigned tasks_waiting;
	unsigned tasks_done;
};

struct SetThreads
{
	unsigned short threads;
};

void a3d_async_planner_destroy(Planner& planner);
} // namespace Async
} // namespace Components
} // namespace A3D

struct APOKALYPSE3DAPI_EXPORT AsyncTaskComponents
{
	AsyncTaskComponents(flecs::world& world);

	flecs::entity planner_;
	flecs::entity plannerStatus_;
	flecs::entity setThreads_;

	flecs::entity onRemovePlanner_;
};

#endif // ASYNCTASKCOMPONENTS_H
