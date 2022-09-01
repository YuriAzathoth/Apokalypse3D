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

#ifndef GPUPROGRAMCACHESYSTEMS_H
#define GPUPROGRAMCACHESYSTEMS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
struct APOKALYPSE3DAPI_EXPORT GpuProgramCacheSystems
{
	GpuProgramCacheSystems(flecs::world& world);

	flecs::query<> programs_;
	flecs::query<> shaders_;

	flecs::entity findProgram_;
	flecs::entity findShader_;
	flecs::entity loadShaderFile_;
	flecs::entity linkProgram_;
	flecs::entity setProgramAfterLoad_;
	flecs::entity setShaderAfterLoad_;
	flecs::entity clearPrograms_;
	flecs::entity clearShaders_;
};
} // namespace A3D

#endif // GPUPROGRAMCACHESYSTEMS_H
