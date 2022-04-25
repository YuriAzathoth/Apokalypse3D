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

#ifndef SCENECOMPONENTS_H
#define SCENECOMPONENTS_H

#include <bgfx/bgfx.h>
#include <glm/ext/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

struct Node
{
	Node()
		: model_(1.0f)
	{
	}
	glm::mat4 model_;
};

struct Scene
{
};

struct Translation
{
	Translation()
		: model_(1.0f)
	{
	}
	Translation(const glm::mat4& model)
		: model_(model)
	{
	}
	glm::mat4 model_;
};

struct Flags
{
	bool update_;
};

struct Camera
{
	glm::mat4 view_;
	glm::mat4 proj_;
};

struct Perspective
{
	float fov_;
	float aspect_;
	float near_;
	float far_;
};

struct Ray
{
	glm::vec3 begin_;
	glm::vec3 end_;
};

struct Model
{
	bgfx::VertexBufferHandle vbo_;
	bgfx::IndexBufferHandle ebo_;
	bgfx::ProgramHandle program_;
};

struct Look
{
	glm::vec3 move_;
};

struct Move
{
	glm::vec3 move_;
};

struct Rotate
{
	glm::vec3 rot_;
};

#endif // SCENECOMPONENTS_H
