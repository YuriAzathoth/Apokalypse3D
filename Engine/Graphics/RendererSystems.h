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

#ifndef RENDERERSYSTEMS_H
#define RENDERERSYSTEMS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
struct APOKALYPSE3DAPI_EXPORT RendererSystems
{
	RendererSystems(flecs::world& world);

	flecs::entity initDefault_;
	flecs::entity initRenderer_;
	flecs::entity destroy_;
	flecs::entity frameBegin_;
	flecs::entity frameEnd_;
	flecs::entity threadsBegin_;
	flecs::entity threadsEnd_;
	flecs::entity updateCameraView_;

	flecs::entity updateAspect_;
	flecs::entity addThreads_;
	flecs::entity removeThreads_;
};
} // namespace A3D

#endif // RENDERERSYSTEMS
