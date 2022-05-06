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
#include <glm/vec3.hpp>

// =========================================
// Transformations
// =========================================

struct Look
{
	glm::vec3 move;
};

struct Move
{
	glm::vec3 move;
};

struct Rotate
{
	glm::vec3 rot;
};

// =========================================
// Scene
// =========================================

struct Node
{
	Node()
		: model(1.0f)
	{
	}
	glm::mat4 model;
};

struct Scene
{
};

struct Translation
{
	Translation()
		: model(1.0f)
	{
	}
	Translation(const glm::mat4& _model)
		: model(_model)
	{
	}
	glm::mat4 model;
};

#endif // SCENECOMPONENTS_H
