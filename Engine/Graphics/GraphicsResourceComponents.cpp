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

#include "GraphicsResourceComponents.h"

using namespace A3D::Components::Resource::Graphics;
using namespace A3D::Components::Str;

namespace A3D
{
GraphicsResourceComponents::GraphicsResourceComponents(flecs::world& world)
{
	world.import<StringComponents>();

	flecs::_::cpp_type<GraphicsResourceComponents>::id_explicit(world, 0, false);
	world.module<GraphicsResourceComponents>("A3D::Components::Resource::Graphics");

	// Model
	getModelFile_ = world.component<GetModelFile>().member<StringView>("filename");
	meshGroup_ = world.component<MeshGroup>();
	meshPrimitive_ = world.component<MeshPrimitive>();
	meshStorage_ = world.component<MeshStorage>().add(flecs::Tag);

	// GPU Program
	getGpuProgram_ = world.component<GetGpuProgram>();//.member<StringView>("vertex").member<StringView>("fragment");
	gpuProgram_ = world.component<GpuProgram>().member<uint16_t>("handle");
	gpuProgramStorage_ = world.component<GpuProgramStorage>().add(flecs::Tag);
	program_ = world.component<Program>().add(flecs::Tag);

	// Shader
	getShader_ = world.component<GetGpuProgram>().member<StringView>("filename");
	shader_ = world.component<Shader>().member<uint16_t>("handle");
	shaderStorage_ = world.component<ShaderStorage>().add(flecs::Tag);

	// Texture
	getTexture_ = world.component<GetTexture>().member<StringView>("filename");
	texture_ = world.component<Texture>().member<uint16_t>("handle");
	textureStorage_ = world.component<TextureStorage>().add(flecs::Tag);

	// Private
	linkGpuProgram_ = world.component<LinkGpuProgram>().add(flecs::Tag);
	loadModelFile_ = world.component<LoadModelFile>().add(flecs::Tag);
	loadShaderFile_ = world.component<LoadShaderFile>().add(flecs::Tag);
	loadTextureFile_ = world.component<LoadTextureFile>().add(flecs::Tag);
	setAfterLoad_ = world.component<SetAfterLoad>().add(flecs::Tag);
	shaderFragment_ = world.component<ShaderFragment>().add(flecs::Tag);
	shaderVertex_ = world.component<ShaderVertex>().add(flecs::Tag);
	vertexLayout_ = world.component<VertexLayout>();
}
} // namespace A3D
