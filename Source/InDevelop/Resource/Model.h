/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022-2024 Yuriy Zinchenko

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

#ifndef RESOURCE_MODEL_H
#define RESOURCE_MODEL_H

#include "EngineAPI.h"
#include "Common/Geometry.h"
#include "Common/Model.h"

namespace A3D
{
ENGINEAPI_EXPORT bool GetModel(Model& model, const char* filename);
ENGINEAPI_EXPORT void ReleaseModel(const char* filename);
} // namespace A3D

#endif // RESOURCE_MODEL_H
