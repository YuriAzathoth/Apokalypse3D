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

#ifndef GRAPHICS_GEOMETRY_H
#define GRAPHICS_GEOMETRY_H

#include <bgfx/bgfx.h>
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

struct MeshGroup
{
	bgfx::IndexBufferHandle ebo;
	bgfx::VertexBufferHandle vbo;
};

struct MeshPrimitive
{
	uint32_t start_index;
	uint32_t indices_count;
	uint16_t start_vertex;
	uint16_t vertices_count;
};
} // namespace A3D

#endif // GRAPHICS_GEOMETRY_H
