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

#include "AsyncLoaderComponents.h"

using namespace A3D::Components::AsyncLoader;

namespace A3D
{
AsyncLoaderComponents::AsyncLoaderComponents(flecs::world& world)
{
	flecs::_::cpp_type<AsyncLoaderComponents>::id_explicit(world, 0, false);
	world.module<AsyncLoaderComponents>("A3D::Components::AsyncLoader");

	setAfterLoad_ = world.component<SetAfterLoad>().add(flecs::Tag);
}
} // namespace A3D
