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

#ifndef SCENE_H
#define SCENE_H

#include <flecs.h>
#include <glm/mat4x4.hpp>

class Coordinator;

class Scene
{
public:
	explicit Scene(flecs::world& world, const char* sceneName);
	~Scene();

	void InitSystems(flecs::world& world);
	void Update(flecs::entity& node, const glm::mat4& parent) noexcept;
	void Update() noexcept { Update(root_, glm::mat4{1.0f}); }

	flecs::entity GetRoot() noexcept { return root_; }
	const flecs::entity GetRoot() const noexcept { return root_; }

private:
	flecs::entity root_;
};

#endif // SCENE_H
