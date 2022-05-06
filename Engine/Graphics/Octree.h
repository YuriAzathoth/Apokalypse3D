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

#ifndef OCTREE_H
#define OCTREE_H

#include <flecs.h>
#include <glm/vec3.hpp>
#include "Apokalypse3DAPI.h"

class APOKALYPSE3DAPI_EXPORT Octree
{
public:
	explicit Octree(flecs::world& world, const glm::vec3& center, float size);

	void Insert(flecs::entity entity, const glm::vec3& pos, const glm::vec3& size);
	void Clear();
	void Find(flecs::vector<flecs::entity>& result, const glm::vec3& pos, float range);

private:
	struct Entity
	{
		flecs::entity entity;
		glm::vec3 pos;
		glm::vec3 size;
	};

	struct Node
	{
		flecs::vector<flecs::entity> entities;
		Node* children[8];
		Node* parent;
		int id_;
		bool isLeaf_;
	};

	Node* AddNode(Node* parent);

	flecs::vector<flecs::entity> nodes_;
	flecs::vector<flecs::entity> freeNodes_;
	flecs::entity root_;
	glm::vec3 center_;
	float size_;
	unsigned count_;
};

#endif // OCTREE_H
