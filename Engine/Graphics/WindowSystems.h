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

#ifndef WINDOWSYSTEMS_H
#define WINDOWSYSTEMS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
struct APOKALYPSE3DAPI_EXPORT WindowSystems
{
	WindowSystems(flecs::world& world);

	flecs::entity initDefault_;
	flecs::entity initVideo_;
	flecs::entity initWindow_;
	flecs::entity destroyWindow_;
	flecs::entity destroyVideo_;
};
} // namespace A3D

#endif // WINDOWSYSTEMS_H
