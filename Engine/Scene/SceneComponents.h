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
#include <flecs.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Scene
{
struct Look
{
	glm::vec3 move;
};

struct Move
{
	glm::vec3 move;
};

struct Node
{
	glm::mat4 model;
	Node() : model(1.0f) {}
};

struct Rotate
{
	glm::vec3 rot;
};

struct Translation
{
	glm::mat4 model;
	Translation() : model(1.0f) {}
	Translation(const glm::mat4& _model) : model(_model) {}
};

struct Root {};
} // namespace Scene
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT SceneComponents
{
	SceneComponents(flecs::world& world);

	flecs::entity look_;
	flecs::entity move_;
	flecs::entity node_;
	flecs::entity root_;
	flecs::entity rotate_;
	flecs::entity translation_;
};
} // namespace A3D

#endif // SCENECOMPONENTS_H
