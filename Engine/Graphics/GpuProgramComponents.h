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

#ifndef GPUPROGRAMCOMPONENTS_H
#define GPUPROGRAMCOMPONENTS_H

#include <bgfx/bgfx.h>
#include <flecs.h>
#include "Apokalypse3DAPI.h"
#include "Core/StringComponents.h"

namespace A3D
{
namespace Components
{
namespace GpuProgram
{
struct GetProgram
{
	Str::StringView vertex;
	Str::StringView fragment;
};

struct GetShader
{
	Str::StringView filename;
};

struct Program
{
	bgfx::ProgramHandle handle;
};

struct Shader
{
	bgfx::ShaderHandle handle;
};
} // namespace GpuProgram
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT GpuProgramComponents
{
	GpuProgramComponents(flecs::world& world);

	flecs::entity getProgram_;
	flecs::entity getShader_;
	flecs::entity program_;
	flecs::entity shader_;
};
} // namespace A3D

#endif // GPUPROGRAMCOMPONENTS_H
