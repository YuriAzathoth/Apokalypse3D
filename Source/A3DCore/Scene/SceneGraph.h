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

#ifndef SCENE_SCENE_GRAPH_H
#define SCENE_SCENE_GRAPH_H

#include <stdint.h>
#include "A3DCoreAPI.h"
#include "Common/Geometry.h"
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/vector.h"

namespace A3D
{
using NodeHandleId = uint16_t;

struct NodeHandle
{
	NodeHandleId handle;
};

class A3DCOREAPI_EXPORT SceneGraph
{
public:
	using SizeType = NodeHandleId;

	SceneGraph();
	~SceneGraph() {}

	NodeHandle AddNode();
	NodeHandle AddNode(NodeHandle parent);

	void RemoveNode(NodeHandle node);

	const bool IsNodeExists(NodeHandle node) const noexcept
	{
		return ids_.contains(node.handle);
	}

	const mat4& GetGlobalTransform(NodeHandle node) const noexcept
	{
		const InternalNodeKey key = ids_[node.handle];
		return generations_[key.generation].global_transforms[key.position].transform;
	}

	SizeType GetGenerationsCount() const noexcept { return generations_.size(); }

	void SetTransform(NodeHandle node, mat4& transform)
	{
		const InternalNodeKey key = ids_[node.handle];
		glm_mat4_copy(transform, GetLocalTransformMatrix(key));
	}

	void Translate(NodeHandle node, mat4 translation)
	{
		/*const NodeHandleInternal key_internal = ids_[node.handle];
		mat4* transform_ptr;
		if (key_internal.level > 0)
			transform_ptr = &local_transforms_[key_internal.level - 1][key_internal.position].transform;
		else
			transform_ptr = &global_transforms_[key_internal.level][key_internal.position].transform;
		glm_mat4_mul(*transform_ptr, translation, *transform_ptr);*/
	}

	void UpdateTransformations();

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

	struct SiblingListNode
	{
		PositionIndex next;
		PositionIndex prev;
	};

	struct Generation
	{
		dense_map<PositionIndex, GlobalTransform> global_transforms;
		dense_map<PositionIndex, Box> bounding_boxes;
		dense_map<PositionIndex, Sphere> bounding_spheres;
		dense_map<PositionIndex, PositionIndex> first_children;
		dense_map<PositionIndex, NodeHandleId> external_handles;
		PositionIndex first_dirty;
	};

	struct GenerationInherited
	{
		dense_map<PositionIndex, LocalTransform> local_transforms;
		dense_map<PositionIndex, PositionIndex> parents;
		dense_map<PositionIndex, SiblingListNode> siblings;
	};

	void RemoveNode(Generation& generation, InternalNodeKey key);
	void RemoveNode(GenerationInherited& inherited, InternalNodeKey key);

	void RebindNodeChildren(Generation& generation, InternalNodeKey key);
	void RebindNodeParentAndSiblings(GenerationInherited& inherited, InternalNodeKey key);

	inline mat4& GetLocalTransformMatrix(InternalNodeKey key) noexcept
	{
		if (key.generation > 0)
			return generations_inherited_[key.generation - 1].local_transforms[key.position].transform;
		else
			return generations_[key.generation].global_transforms[key.position].transform;
	}

	sparse_map<NodeHandleId, InternalNodeKey> ids_;
	dense_map<GenerationIndex, Generation> generations_;
	dense_map<GenerationIndex, GenerationInherited> generations_inherited_;

private:
	SceneGraph(const SceneGraph&) = delete;
	void operator=(const SceneGraph&) = delete;

private:
	static constexpr IndexType EMPTY_KEY = ~static_cast<IndexType>(0);

	static void Swap(Generation& generation, PositionIndex left, PositionIndex right);
	static void Swap(GenerationInherited& inherited, PositionIndex left, PositionIndex right);
};
} // namespace A3D

#endif // SCENE_SCENE_GRAPH_H
