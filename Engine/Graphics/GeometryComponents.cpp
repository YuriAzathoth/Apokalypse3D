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

#include "GeometryComponents.h"

using namespace A3D::Components::Geometry;

namespace A3D
{
GeometryComponents::GeometryComponents(flecs::world& world)
{
	flecs::_::cpp_type<GeometryComponents>::id_explicit(world, 0, false);
	world.module<GeometryComponents>("A3D::Components::Geometry");

	aabb_ = world.component<Aabb>();
	model_ = world.component<Model>().add(flecs::Tag);
	obb_ = world.component<Obb>();
	sphere_ = world.component<Sphere>();
}
} // namespace A3D
