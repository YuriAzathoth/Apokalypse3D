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

#include "A3DGraphicsAPI.h"

namespace A3D
{
struct GpuProgram;

A3DGRAPHICSAPI_EXPORT bool GetGpuProgram(GpuProgram& program, const char* vertex_filename, const char* fragment_filename);
A3DGRAPHICSAPI_EXPORT void ReleaseGpuProgram(GpuProgram& program);

A3DGRAPHICSAPI_EXPORT void UpdateGpuProgramCache();
} // namespace A3D

#endif // RESOURCE_GPU_PROGRAM_H
