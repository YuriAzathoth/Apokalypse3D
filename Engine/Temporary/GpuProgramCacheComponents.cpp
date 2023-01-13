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

#include "GpuProgramCacheComponents.h"

using namespace A3D::Components::Cache::GpuProgram;

A3D::GpuProgramCacheComponents::GpuProgramCacheComponents(flecs::world& world)
{
	world.module<GpuProgramCacheComponents>("A3D::Components::Cache::GpuProgram");

	linkProgram_ = world.component<LinkProgram>().add(flecs::Tag);
	loadShaderFile_ = world.component<LoadShaderFile>().add(flecs::Tag);
	programStorage_ = world.component<ProgramStorage>().add(flecs::Tag);
	shaderFragment_ = world.component<ShaderFragment>().add(flecs::Tag);
	shaderStorage_ = world.component<ShaderStorage>().add(flecs::Tag);
	shaderVertex_ = world.component<ShaderVertex>().add(flecs::Tag);
}
