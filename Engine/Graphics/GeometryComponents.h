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

#ifndef GEOMETRYCOMPONENTS_H
#define GEOMETRYCOMPONENTS_H

#include <bgfx/bgfx.h>
#include <bx/bounds.h>
#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Geometry
{
struct Aabb
{
	bx::Aabb aabb;
};

struct Obb
{
	bx::Obb obb;
};

struct Sphere
{
	bx::Sphere sphere;
};

struct Model {};
} // namespace Geometry
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT GeometryComponents
{
	GeometryComponents(flecs::world& world);

	flecs::entity aabb_;
	flecs::entity loadData_;
	flecs::entity loadFile_;
	flecs::entity model_;
	flecs::entity obb_;
	flecs::entity sphere_;
};
} // namespace A3D

#endif // GEOMETRYCOMPONENTS_H
