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

#ifndef SYSTEMCOMPONENTS_H
#define SYSTEMCOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace System
{
using UpdateFunc = void(flecs::world&);

enum class Phase
{
	Begin,
	Prepare,
	Submit,
	End
};

struct Singletons
{
	flecs::query<> begin;
	flecs::query<> prepare;
	flecs::query<> submit;
	flecs::query<> end;
};

struct System
{
	UpdateFunc* func;
};

struct Singleton {};
} // namespace System
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT SystemComponents
{
	SystemComponents(flecs::world& world);

	flecs::entity phase_;
	flecs::entity singleton_;
	flecs::entity singletons_;
	flecs::entity system_;
};
} // namespace A3D

#endif // SYSTEMCOMPONENTS_H
