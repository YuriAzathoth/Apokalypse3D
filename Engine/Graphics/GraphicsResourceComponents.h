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

#ifndef GRAPHICSRESOURCECOMPONENTS_H
#define GRAPHICSRESOURCECOMPONENTS_H

#include <bgfx/bgfx.h>
#include <bx/bounds.h>
#include <flecs.h>
#include "Apokalypse3DAPI.h"
#include "Core/StringComponents.h"

namespace A3D
{
namespace Components
{
namespace Resource
{
namespace Graphics
{
struct GetGpuProgram
{
	Str::StringView vertex;
	Str::StringView fragment;
};

struct GetModelFile
{
	Str::StringView filename;
};

struct GetShader
{
	Str::StringView filename;
};

struct GetTexture
{
	Str::StringView filename;
};

struct GpuProgram
{
	bgfx::ProgramHandle handle;
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

struct Program
{
	bgfx::ProgramHandle handle;
};

struct Shader
{
	bgfx::ShaderHandle handle;
};

struct Texture
{
	bgfx::TextureHandle handle;
};

struct VertexLayout
{
	bgfx::VertexLayout layout;
};

// Storage tags

struct GpuProgramStorage {};
struct MeshStorage {};
struct ShaderStorage {};
struct TextureStorage {};

// Detail (used in resources cache)

struct LinkGpuProgram {};
struct LoadModelFile {};
struct LoadShaderFile {};
struct LoadTextureFile {};
struct SetAfterLoad {};
struct ShaderFragment {};
struct ShaderVertex {};
} // namespace Graphics
} // namespace Resource
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT GraphicsResourceComponents
{
	GraphicsResourceComponents(flecs::world& world);

	// Mesh
	flecs::entity getModelFile_;
	flecs::entity meshGroup_;
	flecs::entity meshPrimitive_;
	flecs::entity meshStorage_;

	// GPU Program
	flecs::entity getGpuProgram_;
	flecs::entity gpuProgram_;
	flecs::entity gpuProgramStorage_;
	flecs::entity program_;

	// Shader
	flecs::entity getShader_;
	flecs::entity shader_;
	flecs::entity shaderStorage_;

	// Texture
	flecs::entity getTexture_;
	flecs::entity texture_;
	flecs::entity textureStorage_;

	// Private
	flecs::entity linkGpuProgram_;
	flecs::entity loadModelFile_;
	flecs::entity loadShaderFile_;
	flecs::entity loadTextureFile_;
	flecs::entity setAfterLoad_;
	flecs::entity shaderFragment_;
	flecs::entity shaderVertex_;
	flecs::entity vertexLayout_;
};
} // namespace A3D

#endif // GRAPHICSRESOURCECOMPONENTS_H
