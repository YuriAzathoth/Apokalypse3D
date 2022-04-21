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

#include <GLM/ext/matrix_clip_space.hpp>
#include <GLM/gtc/type_ptr.hpp>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bgfx/platform.h>
#include <limits.h>
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
		SDL_SetError("Failed to create SDL window: %s", SDL_GetError());
		return;
	}

#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window_, &wmi))
	{
		SDL_SetError("Could now get SDL SysWM info: %s", SDL_GetError());
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

	solidLayout_.begin()
		.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
		.add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
		.end();

	proj_ = glm::perspective(glm::radians(45.0f),
							 static_cast<float>(initInfo.width) / static_cast<float>(initInfo.height),
							 0.1f,
							 100.0f);

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
}

void Graphics::EndFrame() noexcept { bgfx::frame(); }

void Graphics::Draw(bgfx::ProgramHandle program, bgfx::VertexBufferHandle vbo, bgfx::IndexBufferHandle ebo) noexcept
{
	bgfx::setVertexBuffer(0, vbo);
	bgfx::setIndexBuffer(ebo);
	bgfx::submit(0, program);
}

void Graphics::SetViewMatrix(const glm::mat4& view) noexcept
{
	bgfx::setViewTransform(0, glm::value_ptr(view), glm::value_ptr(proj_));
}

void Graphics::SetTransform(const glm::mat4& model) noexcept { bgfx::setTransform(glm::value_ptr(model)); }

bgfx::ShaderHandle Graphics::LoadShader(const char* filename) noexcept
{
	char filepath[1024];
	sprintf(filepath, "../Data/Shaders/%s/%s", GetShadersPath(), filename);
	FILE* file = fopen(filepath, "rb");
	if (!file)
	{
		SDL_SetError("Could not create shader: file \"%s\" not found.", filename);
		return BGFX_INVALID_HANDLE;
	}

	fseek(file, 0, SEEK_END);
	const unsigned size = static_cast<unsigned>(ftell(file));
	if (!size)
	{
		SDL_SetError("Could not create shader: file \"%s\" is empty.", filename);
		return BGFX_INVALID_HANDLE;
	}
	rewind(file);

	const bgfx::Memory* mem = bgfx::alloc(size + 1);
	fread(mem->data, 1, size, file);
	fclose(file);
	return bgfx::createShader(mem);
}

bgfx::ProgramHandle Graphics::CreateProgram(bgfx::ShaderHandle vertexShader, bgfx::ShaderHandle fragmentShader) noexcept
{
	return bgfx::createProgram({vertexShader}, {fragmentShader}, false);
}

bgfx::VertexBufferHandle Graphics::CreateVertexBuffer(const void* data, unsigned size) noexcept
{
	return bgfx::createVertexBuffer(bgfx::makeRef(data, size), solidLayout_);
}

bgfx::IndexBufferHandle Graphics::CreateIndexBuffer(const unsigned short* data, unsigned size) noexcept
{
	return bgfx::createIndexBuffer(bgfx::makeRef(data, size));
}

const char* Graphics::GetShadersPath() noexcept
{
	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Direct3D9:
		return "dx9";
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12:
		return "dx11";
	case bgfx::RendererType::Metal:
		return "msl";
	case bgfx::RendererType::OpenGLES:
		return "essl";
	case bgfx::RendererType::OpenGL:
		return "glsl";
	case bgfx::RendererType::Vulkan:
		return "spirv";
	default:
		return nullptr;
	}
}
