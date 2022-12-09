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

#ifndef RENDERERSYSTEMS_H
#define RENDERERSYSTEMS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components { namespace Renderer { struct RendererConfig; }}

APOKALYPSE3DAPI_EXPORT bool create_renderer(flecs::world& world);
APOKALYPSE3DAPI_EXPORT bool create_renderer(flecs::world& world, Components::Renderer::RendererConfig& config);

struct APOKALYPSE3DAPI_EXPORT RendererSystems
{
	RendererSystems(flecs::world& world);

	flecs::entity frameBeginSingleThreaded_;
	flecs::entity frameEndSingleThreaded_;
	flecs::entity frameBeginMultiThreaded_;
	flecs::entity frameEndMultiThreaded_;
	flecs::entity unSetRenderer_;
	flecs::entity updateConfig_;
};
} // namespace A3D

#endif // RENDERERSYSTEMS
