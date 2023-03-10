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

#include "Graphics/Window.h"
#include "Graphics/Renderer.h"

using namespace A3D;

int main()
{
	CreateVideo();

	Window wnd;
	WindowResolution wres { 800, 600 };
	CreateWindow(wnd, "Hello, World!", wres, WindowMode::WINDOWED, false);

	WindowPlatformData wpd;
	GetWindowPlatformData(wpd, wnd);

	RendererGpu gpu;
	RendererResolution rres { 800, 600 };
	CreateRenderer(gpu, wpd.window, wpd.display, rres, RendererType::OpenGL, 16, 0, false, false);

	DestroyRenderer();
	DestroyWindow(wnd);
	DestroyVideo();

	return 0;
}
