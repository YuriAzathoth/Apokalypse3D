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

#include "Apokalypse3DAPI.h"

namespace bgfx
{
struct ShaderHandle;
struct ProgramHandle;
}

namespace A3D
{
APOKALYPSE3DAPI_EXPORT const char* GetShaderPrefix();

APOKALYPSE3DAPI_EXPORT bool LoadShaderFromFile(bgfx::ShaderHandle& shader, const char* filename);
APOKALYPSE3DAPI_EXPORT void DestroyShader(bgfx::ShaderHandle& shader);

APOKALYPSE3DAPI_EXPORT bool LinkGpuProgram(bgfx::ProgramHandle& program, const bgfx::ShaderHandle& vertex, const bgfx::ShaderHandle& fragment);
APOKALYPSE3DAPI_EXPORT void DestroyGpuProgram(bgfx::ProgramHandle& program);
} // namespace A3D

#endif // RESOURCE_GPU_PROGRAM_H
