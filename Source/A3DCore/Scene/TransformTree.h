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

#ifndef SCENE_TRANSFORM_TREE_H
#define SCENE_TRANSFORM_TREE_H

#include <stdint.h>
#include "A3DCoreAPI.h"
#include "Common/Geometry.h"
#include "Container/sparse_map.h"
#include "Container/vector.h"

namespace A3D
{
using NodeHandleId = uint16_t;

struct NodeHandle
{
	NodeHandleId handle;
};

class A3DCOREAPI_EXPORT TransformTree
{
public:
	using SizeType = NodeHandleId;

	TransformTree();
	~TransformTree() {}

	NodeHandle AddNode();
	NodeHandle AddNode(NodeHandle parent);

	void RemoveNode(NodeHandle node);

	const bool IsNodeExists(NodeHandle node) const noexcept
	{
		return ids_.contains(node.handle);
	}

	const mat4& GetGlobalTransform(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		return generations_[key.generation].global_transforms[key.position].transform;
	}

	const mat4& GetLocalTransform(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		return GetLocalTransformMatrix(key);
	}

	void SetTransform(NodeHandle node, mat4& transform)
	{
		const InternalNodeKey key = ids_[node.handle];
		glm_mat4_copy(transform, GetLocalTransformMatrix(key));
	}

	void Translate(NodeHandle node, mat4& translation)
	{
		const InternalNodeKey key = ids_[node.handle];
		mat4& transform = GetLocalTransformMatrix(key);
		glm_mat4_mul(transform, translation, transform);
	}

	void UpdateTransformations();

	NodeHandle GetParent(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		if (key.generation == 0)
			return { EMPTY_KEY };
		const Generation& generation = generations_[key.generation];
		const GenerationInherited& inherited = generations_inherited_[key.generation - 1];
		const PositionIndex parent_id = inherited.parents[key.position];
		if (parent_id != EMPTY_KEY)
		{
			const Generation& parent_generation = generations_[key.generation - 1];
			if (parent_id < parent_generation.external_handles.size())
				return { parent_generation.external_handles[parent_id] };
			else
				return { EMPTY_KEY };
		}
		else
			return { EMPTY_KEY };
	}

	NodeHandle GetFirstChild(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		const Generation& generation = generations_[key.generation];
		const PositionIndex first_child_id = generation.first_children[key.position];
		if (first_child_id != EMPTY_KEY)
		{
			const Generation& children_generation = generations_[key.generation + 1];
			if (first_child_id < children_generation.external_handles.size())
				return { children_generation.external_handles[first_child_id] };
			else
				return { EMPTY_KEY };
		}
		else
			return { EMPTY_KEY };
	}

	NodeHandle GetNextSibling(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		if (key.generation == 0)
			return { EMPTY_KEY };
		const Generation& generation = generations_[key.generation];
		const GenerationInherited& inherited = generations_inherited_[key.generation - 1];
		const PositionIndex next_sibling_id = inherited.siblings[key.position].next;
		if (next_sibling_id != EMPTY_KEY)
		{
			const Generation& children_generation = generations_[key.generation + 1];
			if (next_sibling_id < children_generation.external_handles.size())
				return { children_generation.external_handles[next_sibling_id] };
			else
				return { EMPTY_KEY };
		}
		else
			return { EMPTY_KEY };
	}

	NodeHandle GetPrevSibling(NodeHandle node) const
	{
		const InternalNodeKey key = ids_[node.handle];
		if (key.generation == 0)
			return { EMPTY_KEY };
		const Generation& generation = generations_[key.generation];
		const GenerationInherited& inherited = generations_inherited_[key.generation - 1];
		const PositionIndex prev_sibling_id = inherited.siblings[key.position].next;
		if (prev_sibling_id != EMPTY_KEY)
		{
			const Generation& children_generation = generations_[key.generation + 1];
			if (prev_sibling_id < children_generation.external_handles.size())
				return { children_generation.external_handles[prev_sibling_id] };
			else
				return { EMPTY_KEY };
		}
		else
			return { EMPTY_KEY };
	}

	SizeType GetGenerationsCount() const noexcept { return generations_.size(); }
	SizeType GetGenerationSize(SizeType generation_id) const noexcept
	{
		return generations_[generation_id].external_handles.size();
	}

	size_t GetMemoryUsage() const noexcept;

#ifndef NDEBUG
	void DebugPrint();
#endif // NDEBUG

private:
	using IndexType = uint16_t;
	using GenerationIndex = IndexType;
	using PositionIndex = IndexType;

	struct InternalNodeKey
	{
		IndexType generation;
		IndexType position;
	};

	struct Siblings
	{
		PositionIndex next;
		PositionIndex prev;
	};

	struct Generation
	{
		vector<PositionIndex, GlobalTransform> global_transforms;
		vector<PositionIndex, Box> bounding_boxes;
		vector<PositionIndex, Sphere> bounding_spheres;
		vector<PositionIndex, PositionIndex> first_children;
		vector<PositionIndex, NodeHandleId> external_handles;
		PositionIndex first_garbage;
	};

	struct GenerationInherited
	{
		vector<PositionIndex, LocalTransform> local_transforms;
		vector<PositionIndex, PositionIndex> parents;
		vector<PositionIndex, Siblings> siblings;
	};

	void RemoveRootNode(Generation& generation, InternalNodeKey key);
	void RemoveChildNode(Generation& generation, GenerationInherited& inherited, InternalNodeKey key);

	void RemoveChildren(Generation& children_generation,
						GenerationInherited& children_inherited,
						GenerationIndex children_generation_id,
						PositionIndex first_child_id,
						PositionIndex parent_id);

	void SwapNodesInGeneration(Generation& generation, GenerationIndex generation_id, PositionIndex left_id, PositionIndex right_id);
	void SwapNodesInGenerationInherited(GenerationInherited& inherited, GenerationIndex generation_id, PositionIndex left_id, PositionIndex right_id);

	void UpdateChildrenLinks(GenerationInherited& children_inherited, PositionIndex first_child_id, PositionIndex parent_id);
	void UpdateParentLink(Generation& parent_generation, PositionIndex parent_id, PositionIndex old_child_id, PositionIndex new_child_id);
	void UpdateSiblingsLinks(GenerationInherited& inherited, PositionIndex id);

	void CleanGeneration(Generation& generation, PositionIndex first_garbage);
	void CleanGenerationInherited(GenerationInherited& inherited, PositionIndex first_garbage);

	void CleanGarbage();

	inline mat4& GetLocalTransformMatrix(InternalNodeKey key)
	{
		if (key.generation > 0)
			return generations_inherited_[key.generation - 1].local_transforms[key.position].transform;
		else
			return generations_[key.generation].global_transforms[key.position].transform;
	}

	inline const mat4& GetLocalTransformMatrix(InternalNodeKey key) const
	{
		if (key.generation > 0)
			return generations_inherited_[key.generation - 1].local_transforms[key.position].transform;
		else
			return generations_[key.generation].global_transforms[key.position].transform;
	}

	sparse_map<NodeHandleId, InternalNodeKey> ids_;
	vector<GenerationIndex, Generation> generations_;
	vector<GenerationIndex, GenerationInherited> generations_inherited_;

private:
	TransformTree(const TransformTree&) = delete;
	void operator=(const TransformTree&) = delete;

public:
	inline static bool IsValid(NodeHandle node) noexcept { return node.handle != EMPTY_KEY; }

private:
	static constexpr IndexType EMPTY_KEY = ~static_cast<IndexType>(0);

	inline static void SwapBoundingBoxes(Box& left, Box& right);
	inline static void SwapBoundingSpheres(Sphere& left, Sphere& right);
	inline static void SwapExternalHandles(NodeHandleId& left, NodeHandleId& right);
	inline static void SwapGlobalTransforms(GlobalTransform& left, GlobalTransform& right);
	inline static void SwapLocalTransforms(LocalTransform& left, LocalTransform& right);
	inline static void SwapPositions(PositionIndex& left, PositionIndex& right);
	inline static void SwapSiblings(Siblings& left, Siblings& right);
};
} // namespace A3D

#endif // SCENE_TRANSFORM_TREE_H
