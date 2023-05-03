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

#include <cglm/cglm.h>
#include "SceneGraph.h"
#include "IO/Log.h"

namespace A3D
{
SceneGraph::SceneGraph()
{
	generations_.emplace();
	generations_.back().first_dirty = 0;
}

NodeHandle SceneGraph::AddNode()
{
	Generation& generation = generations_[0];

	InternalNodeKey key;
	key.generation = 0;

	key.position = generation.global_transforms.insert({GLM_MAT4_IDENTITY_INIT});
	generation.bounding_boxes.insert({GLM_VEC3_ZERO_INIT, GLM_VEC3_ZERO_INIT});
	generation.bounding_spheres.insert({GLM_VEC3_ZERO_INIT, 0.0f});
	generation.first_children.insert(EMPTY_KEY);

	const NodeHandleId handle = ids_.insert(key);
	generation.external_handles.insert(handle);

//	if (generation.first_dirty < generation.external_handles.size() - 1)
//		Swap(generation, generation.first_dirty, generation.external_handles.size() - 1);
//
//	++generation.first_dirty;

	return { handle };
}

NodeHandle SceneGraph::AddNode(NodeHandle parent)
{
	Assert(ids_.contains(parent.handle), "Trying to attach node to non-existent parent.");
	const InternalNodeKey parent_key = ids_[parent.handle];
	Assert(parent_key.generation < generations_.size(), "Trying to attach node to parent in non-existent generation.");

	InternalNodeKey key;
	key.generation = parent_key.generation + 1;

	// Add new generation level if needed
	if (key.generation >= generations_.size())
	{
		generations_.emplace();
		generations_.back().first_dirty = 0;
		generations_inherited_.emplace();
	}

	Generation& generation = generations_[key.generation];

	// Insert node data to the end
	key.position = generation.global_transforms.insert({GLM_MAT4_IDENTITY_INIT});
	generation.bounding_boxes.insert({GLM_VEC3_ZERO_INIT, GLM_VEC3_ZERO_INIT});
	generation.bounding_spheres.insert({GLM_VEC3_ZERO_INIT, 0.0f});
	generation.first_children.insert(EMPTY_KEY);

	// Write external handle
	const NodeHandleId handle = ids_.insert(key);
	generation.external_handles.insert(handle);

	// Insert inherited node data to the end
	GenerationInherited& inherited = generations_inherited_[key.generation - 1];
	inherited.local_transforms.insert({GLM_MAT4_IDENTITY_INIT});
	inherited.parents.insert(parent_key.position);

	PositionIndex& first_child_id = generations_[parent_key.generation].first_children[parent_key.position];

	// Set node as parent's first child
	inherited.siblings.insert({first_child_id, EMPTY_KEY});

	// If parent had children
	if (first_child_id != EMPTY_KEY)
		// Set current node id as parent's first child's previous node index
		inherited.siblings[first_child_id].prev = key.position;

	// Set current node as first parent's child
	first_child_id = key.position;

	return { handle };
}

void SceneGraph::RemoveNode(NodeHandle node)
{
	Assert(ids_.contains(node.handle), "Trying to remove non-existent node.");
	const InternalNodeKey key = ids_[node.handle];
	Assert(key.generation < generations_.size(), "Trying to remove node from non-existent generation.");

	RemoveNode(generations_[key.generation], key);
	if (key.generation > 0)
		RemoveNode(generations_inherited_[key.generation - 1], key);
}

void SceneGraph::RemoveNode(Generation& generation, InternalNodeKey key)
{
	// If generation is not last and node has children
	if ((key.generation + 1 < generations_.size()) && (generation.first_children[key.generation] != EMPTY_KEY))
	{
		InternalNodeKey child_key;
		child_key.generation = key.generation + 1;

		// Remove children
		PositionIndex next_sibling;
		Generation& children_generation = generations_[child_key.generation];
		GenerationInherited& children_inherited = generations_inherited_[child_key.generation - 1];
		for (child_key.position = generation.first_children[key.position];
			 child_key.position != EMPTY_KEY;
			 child_key.position = next_sibling)
		{
			RemoveNode(children_generation, child_key);
			next_sibling = children_inherited.siblings[child_key.position].next;
			if (key.generation > 0)
				RemoveNode(children_inherited, child_key);
		}
	}

	// Erase swap node data
	const PositionIndex rebound_position = generation.global_transforms.erase(key.position);
	generation.bounding_boxes.erase(key.position);
	generation.bounding_spheres.erase(key.position);
	generation.first_children.erase(key.position);

	// Erase external identifier binding
	ids_.erase(generation.external_handles[key.position]);
	generation.external_handles.erase(key.position);

	// If no nodes left in current generation erase generation
	if (generation.external_handles.empty() && key.generation > 0)
		generations_.erase(key.generation);

	// If last node moved to current position
	if (rebound_position != EMPTY_KEY)
	{
		// Set last node external identifier to moved position
		ids_[generation.external_handles[key.position]].position = key.position;

		RebindNodeChildren(generation, key);
	}
}

void SceneGraph::RemoveNode(GenerationInherited& inherited, InternalNodeKey key)
{
	SiblingListNode& sibling_node = inherited.siblings[key.position];

	// If node is not the last parent's child
	if (sibling_node.prev != EMPTY_KEY && sibling_node.next != EMPTY_KEY)
	{
		// "Collapse" siblings
		if (sibling_node.prev != EMPTY_KEY)
			inherited.siblings[sibling_node.prev].next = sibling_node.next;
		if (sibling_node.next != EMPTY_KEY)
			inherited.siblings[sibling_node.next].prev = sibling_node.prev;
	}
	// Else node is the last parent's child
	else
	{
		// Remove this node from parent's first child handle
		const PositionIndex parent_position = inherited.parents[key.position];
		Generation& parent_generation = generations_[key.generation - 1];
		parent_generation.first_children[parent_position] = EMPTY_KEY;
	}

	// Erase swap inherited node data
	const PositionIndex rebound_position = inherited.local_transforms.erase(key.position);
	inherited.parents.erase(key.position);
	inherited.siblings.erase(key.position);

	// If no nodes left in current generation erase generation
	if (inherited.parents.empty())
		generations_inherited_.erase(key.generation - 1);

	// If last node moved to current position
	if (rebound_position != EMPTY_KEY)
		RebindNodeParentAndSiblings(inherited, key);
}

void SceneGraph::RebindNodeChildren(Generation& generation, InternalNodeKey key)
{
	InternalNodeKey child_key;
	child_key.generation = key.generation + 1;

	// Iterate through all children
	PositionIndex next_sibling;
	GenerationInherited& children_inherited = generations_inherited_[child_key.generation];
	for (child_key.position = generation.first_children[key.position];
		 child_key.position != EMPTY_KEY;
		 child_key.position = next_sibling)
	{
		next_sibling = children_inherited.siblings[child_key.position].next;
		generations_inherited_[child_key.generation - 1].parents[child_key.position] = key.position;
	}
}

void SceneGraph::RebindNodeParentAndSiblings(GenerationInherited& inherited, InternalNodeKey key)
{
	SiblingListNode& sibling_node = inherited.siblings[key.position];

	// If sibling is not first
	if (sibling_node.prev != EMPTY_KEY)
		inherited.siblings[sibling_node.prev].next = key.position;
	// Else sibling is first
	else
	{
		// Set new node as first parent's child
		InternalNodeKey parent_key;
		parent_key.generation = key.generation - 1;
		parent_key.position = inherited.parents[key.position];
		generations_[parent_key.generation].first_children[parent_key.position] = key.position;
	}

	// If node has next sibling
	if (sibling_node.next != EMPTY_KEY)
		inherited.siblings[sibling_node.next].prev = key.position;
}

void SceneGraph::UpdateTransformations()
{
	Generation* parent_generation;
	Generation* generation;
	GenerationInherited* inherited;
	mat4* parent_transform;
	mat4* global_transform;
	mat4* local_transform;
	GenerationIndex generation_id;
	PositionIndex position_id;
	for (generation_id = 1; generation_id < generations_.size(); ++generation_id)
	{
		parent_generation = &generations_[generation_id - 1];
		generation = &generations_[generation_id];
		inherited = &generations_inherited_[generation_id - 1];
		for (position_id = 0; position_id < generation->global_transforms.size(); ++position_id)
		{
			parent_transform = &parent_generation->global_transforms[inherited->parents[position_id]].transform;
			global_transform = &generation->global_transforms[position_id].transform;
			local_transform = &inherited->local_transforms[position_id].transform;
			glm_mat4_copy(*parent_transform, *global_transform);
			glm_mat4_mul(*global_transform, *local_transform, *global_transform);
		}
	}
}

size_t SceneGraph::GetMemoryUsage() const noexcept
{
	size_t size = static_cast<size_t>(generations_.memory_size()) * sizeof(Generation) +
				   static_cast<size_t>(generations_inherited_.memory_size()) * sizeof(GenerationInherited);
	for (const Generation& r : generations_)
		size += r.global_transforms.memory_size()
			 + r.bounding_boxes.memory_size()
			 + r.bounding_spheres.memory_size()
			 + r.first_children.memory_size()
			 + r.external_handles.memory_size();
	for (const GenerationInherited& r : generations_inherited_)
		size += r.local_transforms.memory_size()
			 + r.parents.memory_size()
			 + r.siblings.memory_size();
	return size;
}

void SceneGraph::Swap(Generation& generation, PositionIndex left, PositionIndex right)
{
	generation.global_transforms.swap_items(left, right);
	generation.bounding_boxes.swap_items(left, right);
	generation.bounding_spheres.swap_items(left, right);
	generation.first_children.swap_items(left, right);
	generation.external_handles.swap_items(left, right);
}

void SceneGraph::Swap(GenerationInherited& inherited, PositionIndex left, PositionIndex right)
{
	inherited.local_transforms.swap_items(left, right);
	inherited.parents.swap_items(left, right);
	inherited.siblings.swap_items(left, right);
}

#ifndef NDEBUG
void SceneGraph::DebugPrint()
{
	printf("SCENE GRAPH\n");
	printf("  Generations:\n");
	printf("    Size:\t%u\n", generations_.size());
	for (unsigned i = 0; i < generations_.size(); ++i)
	{
		printf("    Generation\t%u\n", i);
		printf("      Size:\t%u\n", generations_[i].external_handles.size());
		printf("      ID\tExternal\tFirst child");
		if (i > 0)
			printf("\tPrev\tNext\tParent");
		printf("\n");
		for (unsigned j = 0; j < generations_[i].external_handles.size() && j < 20; ++j)
		{
			printf("      %u\t\t%u\t\t%u", j, generations_[i].external_handles[j], generations_[i].first_children[j]);
			if (i > 0)
			{
				printf("\t\t%u\t%u\t%u", generations_inherited_[i - 1].siblings[j].prev, generations_inherited_[i - 1].siblings[j].next, generations_inherited_[i - 1].parents[j]);
			}
			printf("\n");
		}
	}
	printf("  External identifiers:\n");
	printf("    Size:\t%u\n", ids_.size());
	printf("    Handle\tGeneration\tPosition\n");
	for (unsigned i = 0; i < ids_.capacity(); ++i)
		if (ids_.contains(i))
			printf("    %u\t\t%u\t\t%u\n", i, ids_[i].generation, ids_[i].position);
}
#endif // NDEBUG
} // namespace A3D
