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

#ifndef GRAPHICSRESOURCESYSTEMS_H
#define GRAPHICSRESOURCESYSTEMS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
struct APOKALYPSE3DAPI_EXPORT GraphicsResourceSystems
{
	GraphicsResourceSystems(flecs::world& world);

	// Model
	flecs::entity findModelFile_;
	flecs::entity loadModelFile_;
	flecs::entity setMeshAfterLoad_;
	flecs::entity destroyMesh_;

	// Shader
	flecs::entity findGpuProgram_;
	flecs::entity findShader_;
	flecs::entity linkGpuProgram_;
	flecs::entity loadShaderFile_;
	flecs::entity compileShader_;
	flecs::entity setGpuProgramAfterLoad_;
	flecs::entity setShaderAfterLoad_;
	flecs::entity destroyGpuProgram_;
	flecs::entity destroyShader_;

	// Texture
	flecs::entity findTexture_;
	flecs::entity loadTextureFile_;
	flecs::entity setTextureAfterLoad_;
	flecs::entity destroyTexture_;

	flecs::query<> gpuProgramStorage_;
	flecs::query<> meshStorage_;
	flecs::query<> shaderStorage_;
	flecs::query<> textureStorage_;
};
} // namespace A3D

#endif // GRAPHICSRESOURCESYSTEMS_H
