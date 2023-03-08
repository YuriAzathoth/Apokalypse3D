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

#ifndef MESHCOMPONENTS_H
#define MESHCOMPONENTS_H

#include <bgfx/bgfx.h>
#include <flecs.h>
#include "Apokalypse3DAPI.h"
#include "Core/StringComponents.h"

namespace A3D
{
namespace Components
{
namespace Mesh
{
struct GetModelFile
{
	Str::StringView filename;
};

struct MeshGroup
{
	bgfx::VertexBufferHandle vbo;
	bgfx::IndexBufferHandle ebo;
	unsigned indicesCount;
	unsigned short verticesCount;
};

struct MeshPrimitive
{
	unsigned startVertex;
	unsigned startIndex;
	unsigned short verticesCount;
	unsigned indicesCount;
};

struct VertexLayout
{
	bgfx::VertexLayout layout;
};
} // namespace Mesh
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT MeshComponents
{
	MeshComponents(flecs::world& world);

	flecs::entity getModelFile_;
	flecs::entity meshGroup_;
	flecs::entity meshPrimitive_;
	flecs::entity vertexLayout_;
};
} // namespace A3D

#endif // MESHCOMPONENTS_H
