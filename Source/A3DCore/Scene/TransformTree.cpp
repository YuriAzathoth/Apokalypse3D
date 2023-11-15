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
#include "Core/EngineLog.h"
#include "TransformTree.h"

namespace A3D
{
TransformTree::TransformTree()
{
	generations_.emplace_back();
	generations_.back().first_garbage = 0;
}

NodeHandle TransformTree::AddNode()
{
	Generation& generation = generations_[0];

	InternalNodeKey key;
	key.generation = 0;
	key.position = generation.global_transforms.size();

	generation.global_transforms.push_back({GLM_MAT4_IDENTITY_INIT});
	generation.bounding_boxes.push_back({GLM_VEC3_ZERO_INIT, GLM_VEC3_ZERO_INIT});
	generation.bounding_spheres.push_back({GLM_VEC3_ZERO_INIT, 0.0f});
	generation.first_children.push_back(EMPTY_KEY);

	const NodeHandleId handle = ids_.insert(key);
	generation.external_handles.push_back(handle);

	++generation.first_garbage;
	Assert(generation.first_garbage == generation.global_transforms.size(),
		   "Failed to add root node tn TransformTree: first_garbage link unsynchronized.");

	return { handle };
}

NodeHandle TransformTree::AddNode(NodeHandle parent)
{
	Assert(ids_.contains(parent.handle), "Failed to add child node tn TransformTree: parent handle does not exist.");

	const InternalNodeKey parent_key = ids_[parent.handle];
	Assert(parent_key.generation < generations_.size(),
		   "Failed to add child node tn TransformTree: parent generation #%u does not exist.", parent_key.generation);
	Assert(parent_key.position < generations_[parent_key.generation].external_handles.size(),
		   "Failed to add child node tn TransformTree: parent node #%u does not exist.", parent_key.generation);

	InternalNodeKey key;
	key.generation = parent_key.generation + 1;

	// Add new generation level if needed
	if (key.generation >= generations_.size())
	{
		generations_.emplace_back();
		generations_.back().first_garbage = 0;
		generations_inherited_.emplace_back();
	}

	Generation& generation = generations_[key.generation];

	key.position = generation.global_transforms.size();

	// Insert node data to the end
	generation.global_transforms.push_back({GLM_MAT4_IDENTITY_INIT});
	generation.bounding_boxes.push_back({GLM_VEC3_ZERO_INIT, GLM_VEC3_ZERO_INIT});
	generation.bounding_spheres.push_back({GLM_VEC3_ZERO_INIT, 0.0f});
	generation.first_children.push_back(EMPTY_KEY);

	// Write external handle
	const NodeHandleId handle = ids_.insert(key);
	generation.external_handles.push_back(handle);

	// Expand first garbage variable
	++generation.first_garbage;
	Assert(generation.first_garbage == generation.global_transforms.size(),
		   "Failed to add child node tn TransformTree: first_garbage link unsynchronized.");

	// Insert inherited node data to the end
	GenerationInherited& inherited = generations_inherited_[key.generation - 1];
	inherited.local_transforms.push_back({GLM_MAT4_IDENTITY_INIT});
	inherited.parents.push_back(parent_key.position);

	PositionIndex& first_child_id = generations_[parent_key.generation].first_children[parent_key.position];

	// Set node as parent's first child
	inherited.siblings.push_back({first_child_id, EMPTY_KEY});

	// If parent had children
	if (first_child_id != EMPTY_KEY)
		// Set current node id as parent's first child's previous node index
		inherited.siblings[first_child_id].prev = key.position;

	// Set current node as first parent's child
	first_child_id = key.position;

	return { handle };
}

void TransformTree::RemoveNode(NodeHandle node)
{
	Assert(ids_.contains(node.handle),
		   "Failed to remove node from TransformTree: node handle does not exist.");
	const InternalNodeKey key = ids_[node.handle];

	Assert(key.generation < generations_.size(),
		   "Failed to remove node from TransformTree: generation #%u does not exists.", key.generation);

	Generation& generation = generations_[key.generation];
	--generation.first_garbage;
	Assert(!generation.external_handles.empty(),
		   "Failed to remove node from TransformTree: generation #%u is empty.", key.generation);

	if (key.generation == 0)
		RemoveRootNode(generation, key);
	else
	{
		GenerationInherited inherited = generations_inherited_[key.generation - 1];
		RemoveChildNode(generation, inherited, key);
	}

	CleanGarbage();
}

void TransformTree::RemoveRootNode(Generation& generation, InternalNodeKey key)
{
	int new_id = (int)generation.first_garbage - 1;
	if ((int)key.position < new_id)
		SwapNodesInGeneration(generation, key.generation, key.position, (PositionIndex)new_id);
	else
		new_id = (int)key.position;

	ids_.erase(generation.external_handles[new_id]);

	const GenerationIndex children_generation_id = key.generation + 1;
	if (children_generation_id < generations_.size())
	{
		RemoveChildren(generations_[children_generation_id],
					   generations_inherited_[children_generation_id - 1],
					   children_generation_id,
					   generation.first_children[new_id],
					   new_id);
	}
}

void TransformTree::RemoveChildNode(Generation& generation, GenerationInherited& inherited, InternalNodeKey key)
{
	int new_id = (int)generation.first_garbage - 1;
	if ((int)key.position < new_id)
	{
		SwapNodesInGeneration(generation, key.generation, key.position, (PositionIndex)new_id);
		SwapNodesInGenerationInherited(inherited, key.generation, key.position, (PositionIndex)new_id);
	}
	else
		new_id = (int)key.position;

	ids_.erase(generation.external_handles[new_id]);

	const GenerationIndex children_generation_id = key.generation + 1;
	if (children_generation_id < generations_.size())
	{
		RemoveChildren(generations_[children_generation_id],
					   generations_inherited_[children_generation_id - 1],
					   children_generation_id,
					   generation.first_children[new_id],
					   new_id);
	}
}

void TransformTree::RemoveChildren(Generation& children_generation,
								GenerationInherited& children_inherited,
								GenerationIndex children_generation_id,
								PositionIndex first_child_id,
								PositionIndex parent_id)
{
	InternalNodeKey child_key;
	child_key.position = first_child_id;
	child_key.generation = children_generation_id;

	PositionIndex next_child_id;

	while (child_key.position != EMPTY_KEY)
	{
		--children_generation.first_garbage;

		next_child_id = children_inherited.siblings[child_key.position].next;
		RemoveChildNode(children_generation, children_inherited, child_key);
		child_key.position = next_child_id;
	}
}

void TransformTree::SwapNodesInGeneration(Generation& generation,
									   GenerationIndex generation_id,
									   PositionIndex left_id,
									   PositionIndex right_id)
{
	Assert(generation_id < generations_.size(),
		   "Failed to swap nodes in TransformTree: generation #%u does not exist.", generation_id);
	Assert(left_id < generation.external_handles.size(),
		   "Failed to swap nodes in TransformTree: node #%u does not exist.", left_id);
	Assert(right_id < generation.external_handles.size(),
		   "Failed to swap nodes in TransformTree: node #%u does not exist.", right_id);
	Assert(left_id != right_id,
		   "Failed to swap nodes in TransformTree: left and right node ids #%u ared same.", left_id);

	SwapGlobalTransforms(generation.global_transforms[left_id], generation.global_transforms[right_id]);
	SwapBoundingBoxes(generation.bounding_boxes[left_id], generation.bounding_boxes[right_id]);
	SwapBoundingSpheres(generation.bounding_spheres[left_id], generation.bounding_spheres[right_id]);
	SwapPositions(generation.first_children[left_id], generation.first_children[right_id]);
	SwapExternalHandles(generation.external_handles[left_id], generation.external_handles[right_id]);

	if (generation_id + 1 < generations_.size())
	{
		GenerationInherited& inherited = generations_inherited_[generation_id - 1];
		UpdateChildrenLinks(inherited, generation.first_children[left_id], left_id);
		UpdateChildrenLinks(inherited, generation.first_children[right_id], right_id);
	}

	ids_[generation.external_handles[left_id]].position = left_id;
	ids_[generation.external_handles[right_id]].position = right_id;
}

void TransformTree::SwapNodesInGenerationInherited(GenerationInherited& inherited, GenerationIndex generation_id, PositionIndex left_id, PositionIndex right_id)
{
	Assert(generation_id > 0,
		   "Failed to swap children nodes in TransformTree: generation 0 does not have any parents.");
	Assert(generation_id < generations_.size(),
		   "Failed to swap children nodes in TransformTree: generation #%u does not exist.", generation_id);
	Assert(left_id < inherited.local_transforms.size(),
		   "Failed to swap children nodes in TransformTree: node #%u does not exist.", left_id);
	Assert(right_id < inherited.local_transforms.size(),
		   "Failed to swap children nodes in TransformTree: node #%u does not exist.", right_id);
	Assert(left_id != right_id,
		   "Failed to swap children nodes in TransformTree: left and right node ids #%u are same.", left_id);

	SwapLocalTransforms(inherited.local_transforms[left_id], inherited.local_transforms[right_id]);
	SwapPositions(inherited.parents[left_id], inherited.parents[right_id]);
	SwapSiblings(inherited.siblings[left_id], inherited.siblings[right_id]);

	UpdateSiblingsLinks(inherited, left_id);
	UpdateSiblingsLinks(inherited, right_id);

	Generation& parent_generation = generations_[generation_id - 1];
	UpdateParentLink(parent_generation, inherited.parents[left_id], left_id, right_id);
	UpdateParentLink(parent_generation, inherited.parents[right_id], right_id, left_id);
}

void TransformTree::UpdateChildrenLinks(GenerationInherited& children_inherited, PositionIndex first_child_id, PositionIndex parent_id)
{
	for (PositionIndex child_id = children_inherited.parents[first_child_id];
		 child_id != EMPTY_KEY;
		 child_id = children_inherited.siblings[child_id].next)
	{
		children_inherited.parents[child_id] = parent_id;
	}
}

void TransformTree::UpdateParentLink(Generation& parent_generation, PositionIndex parent_id, PositionIndex old_child_id, PositionIndex new_child_id)
{
	Assert(!parent_generation.external_handles.empty(),
		   "Failed to update link to parent in TransformTree: parent generation %u is empty.", parent_id);
	Assert(parent_id < parent_generation.external_handles.size(),
		   "Failed to update link to parent in TransformTree: parent %u does not exist.", parent_id);

	PositionIndex& first_child_id = parent_generation.first_children[parent_id];

	Assert(first_child_id != EMPTY_KEY,
		   "Failed to update link to parent in TransformTree: parent #u does not have any children.", parent_id);

	if (first_child_id == old_child_id)
		first_child_id = new_child_id;
}

void TransformTree::UpdateSiblingsLinks(GenerationInherited& inherited, PositionIndex id)
{
	const Siblings& siblings = inherited.siblings[id];
	if (siblings.prev != EMPTY_KEY)
		inherited.siblings[siblings.prev].next = id;
	if (siblings.next != EMPTY_KEY)
		inherited.siblings[siblings.next].prev = id;
}

void TransformTree::CleanGeneration(Generation& generation, PositionIndex first_garbage)
{
	generation.global_transforms.shrink(first_garbage);
	generation.bounding_boxes.shrink(first_garbage);
	generation.bounding_spheres.shrink(first_garbage);
	generation.first_children.shrink(first_garbage);
	generation.external_handles.shrink(first_garbage);

	generation.first_garbage = generation.global_transforms.size();
}

void TransformTree::CleanGenerationInherited(GenerationInherited& inherited, PositionIndex first_garbage)
{
	inherited.local_transforms.shrink(first_garbage);
	inherited.parents.shrink(first_garbage);
	inherited.siblings.shrink(first_garbage);
}

void TransformTree::CleanGarbage()
{
	Generation* generation = generations_.begin();
	if (generation->first_garbage < generation->external_handles.size())
		CleanGeneration(*generation, generation->first_garbage);
	++generation;

	for(GenerationInherited* inherited = generations_inherited_.begin();
		inherited != generations_inherited_.end();
		++generation, ++inherited)
	{
		if (generation->first_garbage < generation->external_handles.size())
		{
			CleanGeneration(*generation, generation->first_garbage);
			CleanGenerationInherited(*inherited, generation->first_garbage);
		}
	}

	generation = &generations_[1];
	for (GenerationIndex i = 1; i < generations_.size(); ++i, ++generation)
		if (generation->global_transforms.empty())
		{
			generations_.shrink(i);
			generations_inherited_.shrink(i - 1);
			break;
		}
}

void TransformTree::UpdateTransformations()
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

void TransformTree::SwapBoundingBoxes(Box& left, Box& right)
{
	vec3 temp;

	glm_vec3_copy(right.min, temp);
	glm_vec3_copy(left.min, right.min);
	glm_vec3_copy(temp, left.min);

	glm_vec3_copy(right.min, temp);
	glm_vec3_copy(left.min, right.min);
	glm_vec3_copy(temp, left.min);
}

void TransformTree::SwapBoundingSpheres(Sphere& left, Sphere& right)
{
	vec3 tempv;
	glm_vec3_copy(right.center, tempv);
	glm_vec3_copy(left.center, right.center);
	glm_vec3_copy(tempv, left.center);

	float tempf;
	tempf = right.radius;
	right.radius = left.radius;
	left.radius = tempf;
}

void TransformTree::SwapExternalHandles(NodeHandleId& left, NodeHandleId& right)
{
	NodeHandleId temp;
	temp = right;
	right = left;
	left = temp;
}

void TransformTree::SwapGlobalTransforms(GlobalTransform& left, GlobalTransform& right)
{
	mat4 temp;
	glm_mat4_copy(right.transform, temp);
	glm_mat4_copy(left.transform, right.transform);
	glm_mat4_copy(temp, left.transform);
}

void TransformTree::SwapLocalTransforms(LocalTransform& left, LocalTransform& right)
{
	mat4 temp;
	glm_mat4_copy(right.transform, temp);
	glm_mat4_copy(left.transform, right.transform);
	glm_mat4_copy(temp, left.transform);
}

void TransformTree::SwapPositions(PositionIndex& left, PositionIndex& right)
{
	PositionIndex temp;
	temp = right;
	right = left;
	left = temp;
}

void TransformTree::SwapSiblings(Siblings& left, Siblings& right)
{
	PositionIndex temp;

	temp = right.prev;
	right.prev = left.prev;
	left.prev = temp;

	temp = right.next;
	right.next = left.next;
	left.next = temp;
}

size_t TransformTree::GetMemoryUsage() const noexcept
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

#ifndef NDEBUG
void TransformTree::DebugPrint()
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
