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

#ifndef GPUPROGRAMCACHECOMPONENTS_H
#define GPUPROGRAMCACHECOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Cache
{
namespace GpuProgram
{
struct LinkProgram {};
struct LoadShaderFile {};
struct ProgramStorage {};
struct ShaderFragment {};
struct ShaderStorage {};
struct ShaderVertex {};
} // namespace Graphics
} // namespace Resource
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT GpuProgramCacheComponents
{
	GpuProgramCacheComponents(flecs::world& world);

	flecs::entity linkProgram_;
	flecs::entity loadShaderFile_;
	flecs::entity programStorage_;
	flecs::entity shaderFragment_;
	flecs::entity shaderStorage_;
	flecs::entity shaderVertex_;
};
} // namespace A3D

#endif // GPUPROGRAMCACHECOMPONENTS_H
