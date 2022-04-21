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

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <stdio.h>
#include <string.h>
#include "Graphics.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

Graphics::Graphics(const InitInfo initInfo, bool& initialized)
	: window_(nullptr)
{
	initialized = false;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		SDL_SetError("Failed to init SDL video subsystem: %s", SDL_GetError());
		return;
	}

	window_ = SDL_CreateWindow(initInfo.title,
							   SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED,
							   initInfo.width,
							   initInfo.height,
							   SDL_WINDOW_SHOWN);
	if (!window_)
	{
		SDL_SetError("Failed to create SDL window : %s", SDL_GetError());
		return;
	}

#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window_, &wmi))
	{
		SDL_SetError("Could now get SDL SysWM info: ", SDL_GetError());
		return;
	}
#endif // !BX_PLATFORM_EMSCRIPTEN

	bgfx::PlatformData pd;
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	pd.ndt = wmi.info.x11.display;
	pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
	pd.ndt = NULL;
	pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
	pd.ndt = NULL;
	pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_WINDOWS
	pd.ndt = NULL;
	pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_STEAMLINK
	pd.ndt = wmi.info.vivante.display;
	pd.nwh = wmi.info.vivante.window;
#elif BX_PLATFORM_EMSCRIPTEN
	pd.ndt = NULL;
	pd.nwh = (void*)"#canvas";
#endif
	pd.context = NULL;
	pd.backBuffer = NULL;
	pd.backBufferDS = NULL;
	bgfx::setPlatformData(pd);

	bgfx::renderFrame();

	static constexpr bgfx::RendererType::Enum RENDERER_TYPE = bgfx::RendererType::OpenGL;
	static constexpr unsigned short GPU_VENDOR = BGFX_PCI_ID_NONE;

	unsigned resetFlags = 0;
	if (initInfo.vsync)
		resetFlags |= BGFX_RESET_VSYNC;

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = RENDERER_TYPE;
	bgfxInfo.vendorId = GPU_VENDOR;
	bgfxInfo.resolution.width = initInfo.width;
	bgfxInfo.resolution.height = initInfo.height;
	bgfxInfo.resolution.reset = resetFlags;
	if (!bgfx::init(bgfxInfo))
	{
		SDL_SetError("Failed to init BGFX");
		return;
	}

#ifndef NDEBUG
	bgfx::setDebug(BGFX_DEBUG_TEXT);
#endif // NDEBUG

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0xff, 1.0f, 0);

	initialized = true;
}

Graphics::~Graphics()
{
	SDL_StopTextInput();
	bgfx::shutdown();
	SDL_DestroyWindow(window_);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Graphics::BeginFrame() noexcept
{
	int width, height;
	SDL_GetWindowSize(window_, &width, &height);
	bgfx::setViewRect(0, 0, 0, width, height);
	bgfx::touch(0);

	bgfx::renderFrame();
}

void Graphics::EndFrame() noexcept { bgfx::frame(); }
