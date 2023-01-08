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

#include <doctest/doctest.h>
#include <flecs.h>
#include "Core/AsyncTaskComponents.h"
#include "Core/AsyncTaskSystems.h"

using namespace A3D::Components::Async;

namespace Test
{
struct AsyncTaskDoneTag {};
}

struct AsyncTaskTest
{
	explicit AsyncTaskTest(flecs::world& w)
	{
		using namespace Test;
		w.module<AsyncTaskTest>("Test");
		tag_ = w.component<AsyncTaskDoneTag>();
	}
	flecs::entity tag_;
};

TEST_SUITE("Async Thread")
{
//	TEST_CASE("Create 0 Threads")
//	{
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		a3d_async_planner_set(w, 0);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		const Planner* planner = w.get<Planner>();
//		REQUIRE(planner->bufmem == nullptr);
//		REQUIRE(planner->tasks == nullptr);
//		REQUIRE(planner->threads == nullptr);
//		REQUIRE(planner->threads_count == 0);
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//	}
//	TEST_CASE("Create 1 Thread")
//	{
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		a3d_async_planner_set(w, 1);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		const Planner* planner = w.get<Planner>();
//		REQUIRE(planner->bufmem != nullptr);
//		REQUIRE(planner->tasks != nullptr);
//		REQUIRE(planner->threads != nullptr);
//		REQUIRE(planner->threads_count == 1);
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//	}
//	TEST_CASE("Create 2 Threads")
//	{
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		a3d_async_planner_set(w, 2);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		const Planner* planner = w.get<Planner>();
//		REQUIRE(planner->bufmem != nullptr);
//		REQUIRE(planner->tasks != nullptr);
//		REQUIRE(planner->threads != nullptr);
//		REQUIRE(planner->threads_count == 2);
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//	}
//	TEST_CASE("Create 8 Threads")
//	{
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		a3d_async_planner_set(w, 8);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		const Planner* planner = w.get<Planner>();
//		REQUIRE(planner->bufmem != nullptr);
//		REQUIRE(planner->tasks != nullptr);
//		REQUIRE(planner->threads != nullptr);
//		REQUIRE(planner->threads_count == 8);
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//	}
//	TEST_CASE("Resize Threads")
//	{
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		a3d_async_planner_set(w, 2);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		const Planner* planner = w.get<Planner>();
//		REQUIRE(planner->bufmem != nullptr);
//		REQUIRE(planner->tasks != nullptr);
//		REQUIRE(planner->threads != nullptr);
//		REQUIRE(planner->threads_count == 2);
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//		a3d_async_planner_set(w, 4);
//		REQUIRE(w.has<Planner>());
//		REQUIRE(w.has<PlannerStatus>());
//		planner = w.get<Planner>();
//		REQUIRE(planner->bufmem != nullptr);
//		REQUIRE(planner->tasks != nullptr);
//		REQUIRE(planner->threads != nullptr);
//		REQUIRE(planner->threads_count == 4);
//		status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(status->tasks_done == 0);
//	}
//	TEST_CASE("Add 1 task 1 thread")
//	{
//		const unsigned threads = 1;
//		const unsigned tasks = 1;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
//	TEST_CASE("Add 1 task 4 threads")
//	{
//		const unsigned threads = 4;
//		const unsigned tasks = 1;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
//	TEST_CASE("Add 8 tasks 1 thread")
//	{
//		const unsigned threads = 1;
//		const unsigned tasks = 8;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
//	TEST_CASE("Add 8 tasks 2 threads")
//	{
//		const unsigned threads = 2;
//		const unsigned tasks = 8;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
//	TEST_CASE("Add 8 tasks 8 threads")
//	{
//		const unsigned threads = 8;
//		const unsigned tasks = 8;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
//	TEST_CASE("Add 8 tasks 2 threads")
//	{
//		const unsigned threads = 2;
//		const unsigned tasks = 8;
//		using namespace Test;
//		flecs::world w;
//		w.import<AsyncTaskComponents>();
//		w.import<AsyncTaskSystems>();
//		w.import<AsyncTaskTest>();
//		flecs::query result = w.query<const AsyncTaskDoneTag>();
//		a3d_async_planner_set(w, threads);
//		for (unsigned i = 0; i < tasks; ++i)
//			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
//			{
//				stage.entity().add<AsyncTaskDoneTag>();
//				return true;
//			});
//		const PlannerStatus* status = w.get<PlannerStatus>();
//		REQUIRE(status->tasks_waiting == tasks);
//		while (w.progress() && a3d_async_planner_is_done(w))
//			;
//		REQUIRE(status->tasks_waiting == 0);
//		REQUIRE(result.count() == tasks);
//	}
	TEST_CASE("Add 256 tasks 8 threads")
	{
		const unsigned threads = 1;
		const unsigned tasks = 256;
		using namespace Test;
		flecs::world w;
		w.import<AsyncTaskComponents>();
		w.import<AsyncTaskSystems>();
		w.import<AsyncTaskTest>();
		flecs::query result = w.query<const AsyncTaskDoneTag>();
		a3d_async_planner_set(w, threads);
		for (unsigned i = 0; i < tasks; ++i)
			a3d_async_task_add(w, [](const flecs::world& world, flecs::world& stage)
			{
				stage.entity().set<AsyncTaskDoneTag>({});
				return true;
			});
		const PlannerStatus* status = w.get<PlannerStatus>();
		REQUIRE(status->tasks_waiting == tasks);
		while (w.progress() && a3d_async_planner_is_done(w))
			;
		REQUIRE(status->tasks_waiting == 0);
		REQUIRE(result.count() == tasks);
	}
}
