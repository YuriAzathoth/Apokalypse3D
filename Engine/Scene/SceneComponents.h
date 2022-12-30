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

	Position() : position(0.0f, 0.0f, 0.0f) {}
	Position(const glm::vec3& p) : position(p) {}
	Position(float x, float y, float z) : position(x, y, z) {}
	Position(const Position& src) : position(src.position) {}
	void operator=(const Position& src) { position = src.position; }
};

struct Rotation
{
	glm::quat quat;

	Rotation() : quat(1.0f, 0.0f, 0.0f, 0.0f) {}
	Rotation(const glm::quat& q) : quat(q) {}
	Rotation(float angle, const glm::vec3& axis) : quat(glm::angleAxis(angle, axis)) {}
	Rotation(const Rotation& src) : quat(src.quat) {}
	void operator=(const Rotation& src) { quat = src.quat; }
};

struct Scale
{
	glm::vec3 scale;

	Scale() : scale(1.0f, 1.0f, 1.0f) {}
	Scale(const glm::vec3& s) : scale(s) {}
	Scale(float x, float y, float z) : scale(x, y, z) {}
	Scale(const Scale& src) : scale(src.scale) {}
	void operator=(const Scale& src) { scale = src.scale; }
};

struct RelativeTransform
{
	glm::mat4 local;

	RelativeTransform() : local(1.0f) {}
	RelativeTransform(const glm::mat4& t) : local(t) {}
	RelativeTransform(const RelativeTransform& src) : local(src.local) {}
	void operator=(const RelativeTransform& src) { local = src.local; }
};

struct WorldTransform
{
	glm::mat4 global;

	WorldTransform() : global(1.0f) {}
	WorldTransform(const glm::mat4& t) : global(t) {}
	WorldTransform(const WorldTransform& src) : global(src.global) {}
	void operator=(const WorldTransform& src) { global = src.global; }
};

struct Node {};
struct Root {};
struct Static {};
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
	flecs::entity static_;
};
} // namespace A3D

#endif // SCENECOMPONENTS_H
