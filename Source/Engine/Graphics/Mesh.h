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

#ifndef RESOURCE_MESH_H
#define RESOURCE_MESH_H

#include <bgfx/bgfx.h>
#include <bx/macros.h>
#include "Apokalypse3DAPI.h"
#include "Graphics/Geometry.h"

namespace bx
{
	struct AllocatorI;
	struct FileReader;
}

namespace A3D
{
struct MeshGroup;
struct MeshPrimitive;

enum class MeshFileChunk : uint32_t
{
	EOF = 0,
	VERTEX_BUFFER = BX_MAKEFOURCC('V', 'B', ' ', 0x1),
	VERTEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('V', 'B', 'C', 0x0),
	INDEX_BUFFER = BX_MAKEFOURCC('I', 'B', ' ', 0x0),
	INDEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('I', 'B', 'C', 0x1),
	PRIMITIVE = BX_MAKEFOURCC('P', 'R', 'I', 0x0)
};

struct MeshFileReader
{
	bx::FileReader* reader;
	bx::AllocatorI* allocator;
};

APOKALYPSE3DAPI_EXPORT bool OpenMeshFile(MeshFileReader& file, const char* filename);
APOKALYPSE3DAPI_EXPORT void CloseMeshFile(MeshFileReader& file);

APOKALYPSE3DAPI_EXPORT MeshFileChunk GetNextMeshFileChunk(MeshFileReader& file);

APOKALYPSE3DAPI_EXPORT void ReadMeshVertexBuffer(MeshFileReader& file, MeshGroup& group, Box& box, Sphere& sphere, bool compressed);
APOKALYPSE3DAPI_EXPORT void ReadMeshIndexBuffer(MeshFileReader& file, MeshGroup& group, bool compressed);

APOKALYPSE3DAPI_EXPORT bool ReadMeshPrimitiveName(MeshFileReader& file, char* primitive_name, uint16_t max_size);

APOKALYPSE3DAPI_EXPORT uint16_t ReadMeshGroupsCount(MeshFileReader& file);
APOKALYPSE3DAPI_EXPORT bool ReadMeshGroupMaterial(MeshFileReader& file, char* material_name, uint16_t max_size);

APOKALYPSE3DAPI_EXPORT void ReadMeshPrimitive(MeshFileReader& file, MeshPrimitive& primitive, Box& box, Sphere& sphere);
} // namespace A3D

#endif // RESOURCE_MESH_H
