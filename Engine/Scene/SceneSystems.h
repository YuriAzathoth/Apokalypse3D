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

#ifndef SCENESYSTEMS_H
#define SCENESYSTEMS_H

#include "SceneComponents.h"
#include <flecs.h>
#include <glm/mat4x4.hpp>

class Coordinator;
namespace flecs
{
struct world;
}

void RegisterSceneSystems(flecs::world& world, Coordinator& core);

void UpdateScene(flecs::entity node, const Node& parent = Node()) noexcept;

#endif // SCENESYSTEMS_H
