/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2023 Yuriy Zinchenko

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

#ifndef GRAPHICS_DRAW_QUEUE_H
#define GRAPHICS_DRAW_QUEUE_H

#include <atomic>
#include <deque>
#include "Common/Camera.h"
#include "Common/Geometry.h"
#include "Common/GpuProgram.h"
#include "Common/Model.h"

namespace A3D
{
struct DrawQueueItem
{
	const MeshGroup mesh;
	const GpuProgram program;
	const GlobalTransform transform;

	DrawQueueItem(const MeshGroup& _mesh, const GpuProgram& _program, const GlobalTransform& _transform) :
		mesh(_mesh),
		program(_program),
		transform(_transform)
	{
	}

	DrawQueueItem(const DrawQueueItem& other) :
		mesh(other.mesh),
		program(other.program),
		transform(other.transform)
	{
	}
};

enum class DrawQueueState : uint8_t
{
	RECEIVING,
	DRAWING,
	CLEANING
};

struct DrawQueue
{
	using Container = std::deque<DrawQueueItem>;
	Container items;
	std::atomic<DrawQueueState> state;
};

inline static void PushDrawQueue(DrawQueue& queue, const MeshGroup& mesh, const GpuProgram& program, const GlobalTransform& transform)
{
	while (queue.state.load() != DrawQueueState::RECEIVING)
		;
	queue.items.emplace_back(mesh, program, transform);
}

inline static void BeginDrawQueue(DrawQueue& queue)
{
	queue.state.store(DrawQueueState::DRAWING);
}

inline static void EndDrawQueue(DrawQueue& queue)
{
	queue.state.store(DrawQueueState::CLEANING);
	queue.items.clear();
	queue.state.store(DrawQueueState::RECEIVING);
}

inline static
std::pair<DrawQueue::Container::iterator, DrawQueue::Container::iterator>
GetDrawQueueItems(DrawQueue& queue)
{
	if (queue.state.load() == DrawQueueState::DRAWING)
		return std::make_pair(queue.items.begin(), queue.items.end());
	else
		return std::make_pair(queue.items.end(), queue.items.end());
}

void ClearDrawQueue(DrawQueue& queue)
{
	queue.state.store(DrawQueueState::CLEANING);
	queue.items.clear();
}
} // namespace A3D

#endif // GRAPHICS_DRAW_QUEUE_H
