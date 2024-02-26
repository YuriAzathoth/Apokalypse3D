/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2024 Yuriy Zinchenko

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

#ifndef COMMON_GEOMETRY_H
#define COMMON_GEOMETRY_H

#include <cglm/types.h>

namespace A3D
{
struct Box
{
	vec3 min;
	vec3 max;
};

struct Sphere
{
	vec3 center;
	float radius;
};

struct GlobalTransform
{
	mat4 transform;
};

struct LocalTransform
{
	mat4 transform;
};
} // namespace A3D

#endif // COMMON_GEOMETRY_H
