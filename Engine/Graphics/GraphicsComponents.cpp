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

#include "GraphicsComponents.h"

using namespace A3D::Components::Graphics;

A3D::GraphicsComponents::GraphicsComponents(flecs::world& world)
{
	world.module<GraphicsComponents>("A3D::Components::Graphics");

	msaaLevel_ = world.component<MsaaLevel>()
		.constant("NONE", (int)MsaaLevel::NONE)
		.constant("X2", (int)MsaaLevel::X2)
		.constant("X4", (int)MsaaLevel::X4)
		.constant("X8", (int)MsaaLevel::X8)
		.constant("X16", (int)MsaaLevel::X16);

	rendererType_ = world.component<RendererType>()
		.constant("Auto", (int)RendererType::Auto)
		.constant("OpenGL", (int)RendererType::OpenGL)
		.constant("Vulkan", (int)RendererType::Vulkan)
#if defined(__WIN32__)
		.constant("Direct3D9", (int)RendererType::Direct3D9)
		.constant("Direct3D11", (int)RendererType::Direct3D11)
		.constant("Direct3D12", (int)RendererType::Direct3D12)
#elif defined(OSX)
		.constant("Metal", (int)RendererType::Metal)
#endif // defined
		.constant("None", (int)RendererType::None);

	windowMode_ = world.component<WindowMode>()
		.constant("Windowed", (int)WindowMode::Windowed)
		.constant("Fullscreen", (int)WindowMode::Fullscreen)
		.constant("Borderless", (int)WindowMode::Borderless);

	aspect_ = world.component<Aspect>().member<float>("ratio");

	multiThreaded_ = world.component<MultiThreaded>().add(flecs::Tag);
	resizeable_ = world.component<Resizeable>().add(flecs::Tag);

	resolution_ = world.component<Resolution>()
		.member<uint16_t>("width")
		.member<uint16_t>("height");

	vsync_ = world.component<VSync>().add(flecs::Tag);
}
