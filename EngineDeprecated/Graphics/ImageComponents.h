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

#ifndef IMAGECOMPONENTS_H
#define IMAGECOMPONENTS_H

#include <bgfx/bgfx.h>
#include <flecs.h>
#include "Apokalypse3DAPI.h"
#include "Core/StringComponents.h"

namespace A3D
{
namespace Components
{
namespace Image
{
struct GetTexture
{
	Str::StringView filename;
};

struct Texture
{
	bgfx::TextureHandle handle;
};
} // namespace Image
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT ImageComponents
{
	ImageComponents(flecs::world& world);

	flecs::entity getTexture_;
	flecs::entity texture_;
};
} // namespace A3D

#endif // IMAGECOMPONENTS_H
