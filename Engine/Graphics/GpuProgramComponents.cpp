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

#include "GpuProgramComponents.h"

using namespace A3D::Components::GpuProgram;
using namespace A3D::Components::Str;

namespace A3D
{
GpuProgramComponents::GpuProgramComponents(flecs::world& world)
{
	world.import<StringComponents>();

	flecs::_::cpp_type<GpuProgramComponents>::id_explicit(world, 0, false);
	world.module<GpuProgramComponents>("A3D::Components::GpuProgram");

	getProgram_ = world.component<GetProgram>();//.member<StringView>("vertex").member<StringView>("fragment");
	getShader_ = world.component<GetShader>().member<StringView>("filename");
	program_ = world.component<Program>().member<uint16_t>("handle");
	shader_ = world.component<Shader>().member<uint16_t>("handle");
}
} // namespace A3D