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

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include "GlmComponents.h"

using namespace glm;

A3D::GlmComponents::GlmComponents(flecs::world& world)
{
	world.module<GlmComponents>("glm");

	vec2_ = world.component<vec2>().member<float>("x").member<float>("y");
	vec3_ = world.component<vec3>().member<float>("x").member<float>("y").member<float>("z");
	vec4_ = world.component<vec4>().member<float>("x").member<float>("y").member<float>("z").member<float>("w");

	quat_ = world.component<quat>().member<float>("x").member<float>("y").member<float>("z").member<float>("w");

	mat2_ = world.component<mat2>()
		.member<float>("aa").member<float>("ab")
		.member<float>("ba").member<float>("bb");

	mat2x3_ = world.component<mat2x3>()
		.member<float>("aa").member<float>("ab")
		.member<float>("ba").member<float>("bb")
		.member<float>("ca").member<float>("cb");

	mat2x4_ = world.component<mat2x4>()
		.member<float>("aa").member<float>("ab")
		.member<float>("ba").member<float>("bb")
		.member<float>("ca").member<float>("cb")
		.member<float>("da").member<float>("db");

	mat3x2_ = world.component<mat3x2>()
		.member<float>("aa").member<float>("ab").member<float>("ac")
		.member<float>("ba").member<float>("bb").member<float>("bc");

	mat3_ = world.component<mat3>()
		.member<float>("aa").member<float>("ab").member<float>("ac")
		.member<float>("ba").member<float>("bb").member<float>("bc")
		.member<float>("ca").member<float>("cb").member<float>("cc");

	mat3x4_ = world.component<mat3x4>()
		.member<float>("aa").member<float>("ab").member<float>("ac")
		.member<float>("ba").member<float>("bb").member<float>("bc")
		.member<float>("ca").member<float>("cb").member<float>("cc")
		.member<float>("da").member<float>("db").member<float>("dc");

	mat4x2_ = world.component<mat4x2>()
		.member<float>("aa").member<float>("ab").member<float>("ac").member<float>("ad")
		.member<float>("ba").member<float>("bb").member<float>("bc").member<float>("bd");

	mat4x3_ = world.component<mat4x3>()
		.member<float>("aa").member<float>("ab").member<float>("ac").member<float>("ad")
		.member<float>("ba").member<float>("bb").member<float>("bc").member<float>("bd")
		.member<float>("ca").member<float>("cb").member<float>("cc").member<float>("cd");

	mat4_ = world.component<mat4>()
		.member<float>("aa").member<float>("ab").member<float>("ac").member<float>("ad")
		.member<float>("ba").member<float>("bb").member<float>("bc").member<float>("bd")
		.member<float>("ca").member<float>("cb").member<float>("cc").member<float>("cd")
		.member<float>("da").member<float>("db").member<float>("dc").member<float>("dd");
}
