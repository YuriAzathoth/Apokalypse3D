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

#ifndef GRAPHICSCOMPONENTS_H
#define GRAPHICSCOMPONENTS_H

#include <bgfx/bgfx.h>
#include <glm/mat4x4.hpp>

struct Camera
{
	glm::mat4 view;
	glm::mat4 proj;
};

struct MaterialBasic
{
	bgfx::ProgramHandle program;
	bgfx::TextureHandle diffuse;
};

struct Model
{
	bgfx::VertexBufferHandle vbo;
	bgfx::IndexBufferHandle ebo;
};

struct Perspective
{
	float fov;
	float aspect;
	float nearest;
	float farthest;
};

struct Ray
{
	glm::vec3 begin;
	glm::vec3 end;
};

struct WindowAspect
{
};

#endif // GRAPHICSCOMPONENTS_H
