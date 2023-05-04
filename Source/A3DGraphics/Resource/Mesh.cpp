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

#include <bx/allocator.h>
#include <bx/bounds.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include "IO/Log.h"
#include "Mesh.h"

namespace bgfx
{
int32_t read(bx::ReaderI* reader, bgfx::VertexLayout& layout, bx::Error* err);
}

namespace A3D
{
inline static bool IsChunkValid(uint32_t chunk)
{
	return (chunk == (uint32_t)MeshFileChunk::VERTEX_BUFFER) ||
			(chunk == (uint32_t)MeshFileChunk::VERTEX_BUFFER_COMPRESSED) ||
			(chunk == (uint32_t)MeshFileChunk::INDEX_BUFFER) ||
			(chunk == (uint32_t)MeshFileChunk::INDEX_BUFFER_COMPRESSED) ||
			(chunk == (uint32_t)MeshFileChunk::PRIMITIVE);
}

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

bool OpenMeshFile(MeshFileReader& file, const char* filename)
{
	file.allocator = new bx::DefaultAllocator;
	file.reader = new bx::FileReader;
	return bx::open(file.reader, filename);
}

void CloseMeshFile(MeshFileReader& file)
{
	bx::close(file.reader);
	delete file.reader;
	delete file.allocator;
}

MeshFileChunk GetNextMeshFileChunk(MeshFileReader& file)
{
	bx::Error err;
	uint32_t chunk;
	const bool read_success = bx::read(file.reader, chunk, &err) == 4 && err.isOk();
	return (read_success && IsChunkValid(chunk)) ? (static_cast<MeshFileChunk>(chunk)) : MeshFileChunk::END_OF_FILE;
}

void ReadMeshVertexBuffer(MeshFileReader& file, MeshGroup& group, Box& box, Sphere& sphere, bool compressed)
{
	bx::Error err;
	bx::Aabb bx_aabb;
	bx::Obb bx_obb;
	bx::Sphere bx_sphere;

	bx::read(file.reader, bx_sphere, &err);
	bx::read(file.reader, bx_aabb, &err);
	bx::read(file.reader, bx_obb, &err);

	AabbFromBgfx(box, bx_aabb);
	SphereFromBgfx(sphere, bx_sphere);

	bgfx::VertexLayout vertex_layout;
	uint16_t vertices_count;
	bgfx::read(file.reader, vertex_layout, &err);
	bx::read(file.reader, vertices_count, &err);
	const bgfx::Memory* mem = bgfx::alloc(vertices_count * vertex_layout.getStride());

	if (compressed)
	{
		uint32_t compressed_size;
		bx::read(file.reader, compressed_size, &err);

		void* compressed_data = BX_ALLOC(file.allocator, compressed_size);
		bx::read(file.reader, compressed_data, compressed_size, &err);

		meshopt_decodeVertexBuffer(mem->data,
								   vertices_count,
								   vertex_layout.getStride(),
								   (uint8_t*)compressed_data,
								   compressed_size);

		BX_FREE(file.allocator, compressed_data);
	}
	else
		bx::read(file.reader, mem->data, mem->size, &err);

	group.vbo = bgfx::createVertexBuffer(mem, vertex_layout);
}

void ReadMeshIndexBuffer(MeshFileReader& file, MeshGroup& group, bool compressed)
{
	bx::Error err;
	uint32_t indices_count;

	bx::read(file.reader, indices_count, &err);
	const bgfx::Memory* mem = bgfx::alloc(indices_count * sizeof(uint16_t));

	if (compressed)
	{
		uint32_t compressed_size;
		bx::read(file.reader, compressed_size, &err);

		void* compressed_data = BX_ALLOC(file.allocator, compressed_size);
		bx::read(file.reader, compressed_data, compressed_size, &err);

		meshopt_decodeIndexBuffer(mem->data,
								  indices_count,
								  sizeof(uint16_t),
								  (uint8_t*)compressed_data,
								  compressed_size);

		BX_FREE(file.allocator, compressed_data);
	}
	else
		bx::read(file.reader, mem->data, mem->size, &err);

	group.ebo = bgfx::createIndexBuffer(mem);
}

bool ReadMeshPrimitiveName(MeshFileReader& file, char* primitive_name, uint16_t max_size)
{
	bx::Error err;
	uint16_t name_size;
	bx::read(file.reader, name_size, &err);

	if (name_size == 0)
		return true;
	if (name_size <= max_size)
	{
		bx::read(file.reader, primitive_name, &err);
		return true;
	}
	else
		return false;
}

uint16_t ReadMeshGroupsCount(MeshFileReader& file)
{
	bx::Error err;
	uint16_t groups_count;
	bx::read(file.reader, groups_count, &err);
	return groups_count;
}

bool ReadMeshGroupMaterial(MeshFileReader& file, char* material_name, uint16_t max_size)
{
	bx::Error err;
	uint16_t material_size;
	bx::read(file.reader, material_size, &err);

	if (material_size == 0)
		return true;
	if (material_size <= max_size)
	{
		bx::read(file.reader, material_name, &err);
		return true;
	}
	else
		return false;
}

void ReadMeshPrimitive(MeshFileReader& file, MeshPrimitive& primitive, Box& box, Sphere& sphere)
{
	bx::Error err;
	bx::read(file.reader, primitive.start_index, &err);
	bx::read(file.reader, primitive.indices_count, &err);
	bx::read(file.reader, primitive.start_vertex, &err);
	bx::read(file.reader, primitive.vertices_count, &err);

	bx::Aabb bx_aabb;
	bx::Obb bx_obb;
	bx::Sphere bx_sphere;

	bx::read(file.reader, bx_sphere, &err);
	bx::read(file.reader, bx_aabb, &err);
	bx::read(file.reader, bx_obb, &err);

	AabbFromBgfx(box, bx_aabb);
	SphereFromBgfx(sphere, bx_sphere);
}
} // namespace A3D
