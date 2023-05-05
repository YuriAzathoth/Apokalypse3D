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

#ifndef RESOURCE_GPU_PROGRAM_H
#define RESOURCE_GPU_PROGRAM_H

#include <bgfx/bgfx.h>
#include "A3DGraphicsAPI.h"
#include "Common/GpuProgram.h"

namespace A3D
{
struct Shader
{
	bgfx::ShaderHandle handle;
};

A3DGRAPHICSAPI_EXPORT const char* GetShaderPrefix();

A3DGRAPHICSAPI_EXPORT bool LoadShaderFromMemory(Shader& shader, const char* shader_text, uint32_t shader_size);
A3DGRAPHICSAPI_EXPORT bool LoadShaderFromFile(Shader& shader, const char* filename);
A3DGRAPHICSAPI_EXPORT void DestroyShader(Shader& shader);

A3DGRAPHICSAPI_EXPORT bool LinkGpuProgram(GpuProgram& program, const Shader& vertex, const Shader& fragment);
A3DGRAPHICSAPI_EXPORT void DestroyGpuProgram(GpuProgram& program);
} // namespace A3D

#endif // RESOURCE_GPU_PROGRAM_H
