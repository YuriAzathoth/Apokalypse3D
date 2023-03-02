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

#ifndef CAMERACOMPONENTS_H
#define CAMERACOMPONENTS_H

#include <flecs.h>
#include <glm/mat4x4.hpp>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Camera
{
struct Eye
{
	glm::mat4 view;
	glm::mat4 proj;
};

struct Perspective
{
	float fov;
	float nearest;
	float farthest;
};

struct Viewport {};
} // namespace Camera
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT CameraComponents
{
	CameraComponents(flecs::world& world);

	flecs::entity eye_;
	flecs::entity perspective_;
	flecs::entity viewport_;
};
} // namespace A3D

#endif // CAMERACOMPONENTS_H
