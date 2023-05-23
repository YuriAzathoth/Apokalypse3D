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

#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/bounds.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include <unordered_map>
#include "Common/Geometry.h"
#include "Common/Model.h"
#include "Container/dense_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "IO/Log.h"
#include "Model.h"

namespace bgfx
{
int32_t read(bx::ReaderI* reader, bgfx::VertexLayout& layout, bx::Error* err);
}

namespace A3D
{
using MeshIndex = uint32_t;
using ModelIndex = uint32_t;
using RefsType = uint32_t;

inline static constexpr uint32_t CHUNK_VERTEX_BUFFER = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
inline static constexpr uint32_t CHUNK_VERTEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
inline static constexpr uint32_t CHUNK_INDEX_BUFFER = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
inline static constexpr uint32_t CHUNK_INDEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
inline static constexpr uint32_t CHUNK_PRIMITIVE = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

struct MeshCache
{
	std::unordered_map<string_hash, ModelIndex> ids_models;

	dense_map<ModelIndex, Model> models;
	dense_map<ModelIndex, RefsType> refs;
	dense_map<ModelIndex, string> filenames;
};

static MeshCache s_mesh_cache;

inline static void AabbFromBgfx(Box& dst, bx::Aabb& src)
{
	dst.min[0] = src.min.x;
	dst.min[1] = src.min.y;
	dst.min[2] = src.min.z;

	dst.max[0] = src.max.x;
	dst.max[1] = src.max.y;
	dst.max[2] = src.max.z;
}

inline static void SphereFromBgfx(Sphere& dst, bx::Sphere& src)
{
	dst.center[0] = src.center.x;
	dst.center[1] = src.center.y;
	dst.center[2] = src.center.z;

	dst.radius = src.radius;
}

static bool LoadModelFile(Model& model, const char* filename)
{
	bx::FileReader file;
	if (!bx::open(&file, filename))
	{
		LogFatal("Failed to load mesh file \"%s\": file not found.", filename);
		return false;
	}

	bx::DefaultAllocator alloc;
	bx::Error err;
	bx::Aabb bx_aabb;
	bx::Obb bx_obb;
	bx::Sphere bx_sphere;
	bgfx::VertexLayout vertex_layout;
	const bgfx::Memory* mem;
	void* compressed_data;
	char* material_name = nullptr;
	char* primitive_name = nullptr;
	Sphere sphere;
	Box box;
	MeshGroup group;
	MeshPrimitive primitive;
	uint32_t chunk_id;
	uint32_t compressed_size;
	uint32_t indices_count;
	uint16_t vertices_count;
	uint16_t material_size;
	uint16_t primitives_count;
	uint16_t primitive_size;
	uint16_t i;
	uint16_t j;
	while (bx::read(&file, chunk_id, &err) == 4 && err.isOk())
	{
		switch (chunk_id)
		{
		case CHUNK_VERTEX_BUFFER:
			bx::read(&file, bx_sphere, &err);
			bx::read(&file, bx_aabb, &err);
			bx::read(&file, bx_obb, &err);

			AabbFromBgfx(box, bx_aabb);
			SphereFromBgfx(sphere, bx_sphere);

			bgfx::read(&file, vertex_layout, &err);
			bx::read(&file, vertices_count, &err);
			mem = bgfx::alloc(vertices_count * vertex_layout.getStride());

			bx::read(&file, mem->data, mem->size, &err);

			group.vbo = bgfx::createVertexBuffer(mem, vertex_layout);

			break;

		case CHUNK_VERTEX_BUFFER_COMPRESSED:
			bx::read(&file, bx_sphere, &err);
			bx::read(&file, bx_aabb, &err);
			bx::read(&file, bx_obb, &err);

			AabbFromBgfx(box, bx_aabb);
			SphereFromBgfx(sphere, bx_sphere);

			bgfx::read(&file, vertex_layout, &err);
			bx::read(&file, vertices_count, &err);
			mem = bgfx::alloc(vertices_count * vertex_layout.getStride());

			bx::read(&file, compressed_size, &err);

			compressed_data = bx::alloc(&alloc, compressed_size);
			bx::read(&file, compressed_data, compressed_size, &err);

			meshopt_decodeVertexBuffer(mem->data,
									   vertices_count,
									   vertex_layout.getStride(),
									   (uint8_t*)compressed_data,
									   compressed_size);

			bx::free(&alloc, compressed_data);

			group.vbo = bgfx::createVertexBuffer(mem, vertex_layout);

			break;

		case CHUNK_INDEX_BUFFER:
			bx::read(&file, indices_count, &err);
			mem = bgfx::alloc(indices_count * sizeof(uint16_t));

			bx::read(&file, mem->data, mem->size, &err);

			group.ebo = bgfx::createIndexBuffer(mem);

			break;

		case CHUNK_INDEX_BUFFER_COMPRESSED:
			bx::read(&file, indices_count, &err);
			mem = bgfx::alloc(indices_count * sizeof(uint16_t));

			bx::read(&file, compressed_size, &err);

			compressed_data = bx::alloc(&alloc, compressed_size);
			bx::read(&file, compressed_data, compressed_size, &err);

			meshopt_decodeIndexBuffer(mem->data,
									  indices_count,
									  sizeof(uint16_t),
									  (uint8_t*)compressed_data,
									  compressed_size);

			bx::free(&alloc, compressed_data);

			group.ebo = bgfx::createIndexBuffer(mem);

			break;

		case CHUNK_PRIMITIVE:
			bx::read(&file, material_size, &err);

			if (material_size > 0)
			{
				material_name = (char*)bx::alloc(&alloc, material_size + 1);
				bx::read(&file, material_name, material_size, &err);
				material_name[material_size] = '\0';
			}

			// TODO: Set material to mesh group here.

			if (material_size > 0)
				bx::free(&alloc, material_name);

			model.groups.push_back(group);

			bx::read(&file, primitives_count, &err);
			for (j = 0; j < primitives_count; ++j)
			{
				bx::read(&file, primitive_size, &err);

				if (primitive_size > 0)
				{
					primitive_name = (char*)bx::alloc(&alloc, primitive_size + 1);
					bx::read(&file, primitive_name, primitive_size, &err);
					primitive_name[primitive_size] = '\0';
				}

				// TODO: Set mesh primitive name here.

				if (primitive_size > 0)
					bx::free(&alloc, primitive_name);

				bx::read(&file, primitive.start_index, &err);
				bx::read(&file, primitive.indices_count, &err);
				bx::read(&file, primitive.start_vertex, &err);
				bx::read(&file, primitive.vertices_count, &err);

				bx::read(&file, bx_sphere, &err);
				bx::read(&file, bx_aabb, &err);
				bx::read(&file, bx_obb, &err);

				AabbFromBgfx(box, bx_aabb);
				SphereFromBgfx(sphere, bx_sphere);
			}

			break;

		default:
			LogFatal("Failed to read mesh file \"%s\": invalid chunk id.", filename);
			bx::close(&file);
			return false;
		}
	}

	bx::close(&file);

	LogInfo("Mesh file \"%s\" has been loaded.", filename);
	return true;
}

bool GetModel(Model& model, const char* filename)
{
	const auto it = s_mesh_cache.ids_models.find(filename);
	if (it != s_mesh_cache.ids_models.end())
	{
		++s_mesh_cache.refs[it->second];
		model = s_mesh_cache.models[it->second];
		return true;
	}
	else if (LoadModelFile(model, filename))
	{
		s_mesh_cache.ids_models.emplace(filename, s_mesh_cache.models.size());
		s_mesh_cache.models.insert(model);
		s_mesh_cache.refs.insert(1);
		s_mesh_cache.filenames.insert(filename);
		return true;
	}
	else
		return false;
}

void ReleaseModel(const char* filename)
{
	const auto it = s_mesh_cache.ids_models.find(filename);
	if (--s_mesh_cache.refs[it->second] == 0)
	{
		s_mesh_cache.ids_models.erase(filename);

		const ModelIndex index = it->second;
		const ModelIndex rebound = s_mesh_cache.models.erase(index);
		s_mesh_cache.refs.erase(index);
		s_mesh_cache.filenames.erase(index);

		if (rebound != s_mesh_cache.models.INVALID_KEY)
			s_mesh_cache.ids_models[s_mesh_cache.filenames[index]] = index;
	}
}
} // namespace A3D
