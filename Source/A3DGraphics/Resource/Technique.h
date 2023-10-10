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

#ifndef RESOURCE_TECHNIQUE_H
#define RESOURCE_TECHNIQUE_H

#include <bgfx/bgfx.h>
#include "A3DGraphicsAPI.h"
#include "Common/Technique.h"

namespace A3D
{
A3DGRAPHICSAPI_EXPORT bool GetTechnique(Technique& technique, const char* filename);
A3DGRAPHICSAPI_EXPORT void ReleaseTechnique(Technique technique);

bgfx::ProgramHandle GetTechniqueProgram(Technique technique);


struct Uniform
{
	bgfx::UniformHandle handle;
};

A3DGRAPHICSAPI_EXPORT Uniform GetUniform(const char* name);
A3DGRAPHICSAPI_EXPORT uint16_t GetUniformSize(Uniform uniform);
A3DGRAPHICSAPI_EXPORT bgfx::UniformType::Enum GetUniformType(Uniform uniform);
} // namespace A3D

#endif // RESOURCE_TECHNIQUE_H
