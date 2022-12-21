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

#include <flecs.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Scene
{
struct Position
{
	glm::vec3 position;
};

struct Rotation
{
	glm::quat quat;
};

struct Scale
{
	glm::vec3 scale;
};

struct RelativeTransform
{
	glm::mat4 transform;
};

struct WorldTransform
{
	glm::mat4 transform;
};

struct Node {};
struct Root {};
} // namespace Scene
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT SceneComponents
{
	SceneComponents(flecs::world& world);

	flecs::entity relativeTransform_;
	flecs::entity worldTransform_;

	flecs::entity position_;
	flecs::entity rotation_;
	flecs::entity scale_;

	flecs::entity node_;
	flecs::entity root_;
};
} // namespace A3D

#endif // SCENECOMPONENTS_H
