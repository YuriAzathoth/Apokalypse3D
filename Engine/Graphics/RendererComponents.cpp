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

#include "IO/Log.h"
#include "RendererComponents.h"

using namespace A3D::Components::Renderer;

namespace A3D
{
RendererComponents::RendererComponents(flecs::world& world)
{
	world.module<RendererComponents>("A3D::Components::Renderer");

	device_ = world.component<Device>()
		.member<uint16_t>("device")
		.member<uint16_t>("vendor");

	renderer_ = world.component<Renderer>().add(flecs::Tag);
	rendererAllocator_ = world.component<Renderer>();
	rendererThreads_ = world.component<RendererThreads>()
		.on_add([](RendererThreads& threads)
		{
			threads.queues = nullptr;
			threads.count = 0;
		})
		.on_remove([](RendererThreads& threads)
		{
			if (threads.queues)
				free(threads.queues);
		});

	startup_ = world.component<Startup>().add(flecs::Tag);
}
} // namespace A3D
