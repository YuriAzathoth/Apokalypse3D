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
#include <bimg/decode.h>
#include <bgfx/platform.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "Graphics.h"
#include "Scene/SceneComponents.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

static void ReleaseImage([[maybe_unused]]void* ptr, void* data)
{
	bimg::imageFree(reinterpret_cast<bimg::ImageContainer*>(data));
}

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

	bgfx::PlatformData pd{};
#if BX_PLATFORM_LINUX || BX_PLATFORM_BSD
	pd.ndt = wmi.info.x11.display;
	pd.nwh = (void*)(uintptr_t)wmi.info.x11.window;
#elif BX_PLATFORM_OSX
	pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_WINDOWS
	pd.nwh = wmi.info.win.window;
#elif BX_PLATFORM_WINDOWS
	pd.nwh = wmi.info.cocoa.window;
#elif BX_PLATFORM_STEAMLINK
	pd.ndt = wmi.info.vivantnode.display;
	pd.nwh = wmi.info.vivantnode.window;
#elif BX_PLATFORM_EMSCRIPTEN
	pd.nwh = (void*)"#canvas";
#endif
	bgfx::setPlatformData(pd);

	bgfx::renderFrame();

	static constexpr bgfx::RendererType::Enum RENDERER_TYPE = bgfx::RendererType::Vulkan;
	static constexpr unsigned short GPU_VENDOR = BGFX_PCI_ID_NONE;

	unsigned resetFlags = 0;
	if (initInfo.vsync)
		resetFlags |= BGFX_RESET_VSYNC;
	resetFlags |= BGFX_RESET_MSAA_X16;

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
		.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
		.end();

	uniforms_["s_texColor"] = bgfx::createUniform("s_texColor", bgfx::UniformType::Sampler);

	initialized = true;
}

Graphics::~Graphics()
{
	for (auto& handler : uniforms_)
		bgfx::destroy(handler.second);
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

void Graphics::DrawModel(bgfx::ProgramHandle program,
						 bgfx::VertexBufferHandle vbo,
						 bgfx::IndexBufferHandle ebo,
						 const glm::mat4& model) const noexcept
{
	bgfx::setTransform(glm::value_ptr(model));
	bgfx::setVertexBuffer(0, vbo);
	bgfx::setIndexBuffer(ebo);
	bgfx::submit(0, program);
}

void Graphics::DrawScene(flecs::entity node) const noexcept
{
	const Model* model = node.get<Model>();
	const Node* cnode = node.get<Node>();
	if (model && cnode)
	{
		bgfx::setTexture(0, GetUniformHandle("s_texColor"), model->texture_);
		bgfx::setTransform(glm::value_ptr(cnode->model_));
		bgfx::setVertexBuffer(0, model->vbo_);
		bgfx::setIndexBuffer(model->ebo_);
		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::submit(0, model->program_);
	}
	node.children([this](flecs::entity child) { DrawScene(child); });
}

void Graphics::SetTexture(bgfx::TextureHandle texture, unsigned short slot) const noexcept
{
	bgfx::setTexture(slot, GetUniformHandle("s_texColor"), texture);
}

bgfx::ShaderHandle Graphics::LoadShader(const char* filename) noexcept
{
	char filepath[256];
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
		fclose(file);
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
	return bgfx::createProgram(vertexShader, fragmentShader, true);
}

bgfx::VertexBufferHandle Graphics::CreateVertexBuffer(const void* data, unsigned size) noexcept
{
	return bgfx::createVertexBuffer(bgfx::makeRef(data, size), solidLayout_);
}

bgfx::IndexBufferHandle Graphics::CreateIndexBuffer(const unsigned short* data, unsigned size) noexcept
{
	return bgfx::createIndexBuffer(bgfx::makeRef(data, size));
}

bgfx::TextureHandle Graphics::LoadTexture(const char* filename) noexcept
{
	constexpr unsigned long long flags = BGFX_TEXTURE_NONE | BGFX_SAMPLER_NONE;

	char buffer[256];
	sprintf(buffer, "../Data/Textures/%s", filename);
	FILE* file = fopen(buffer, "rb");
	if (!file)
	{
		SDL_SetError("Could not load texture: file \"%s\" not found.", filename);
		return BGFX_INVALID_HANDLE;
	}

	fseek(file, 0, SEEK_END);
	const unsigned size = static_cast<unsigned>(ftell(file));
	if (!size)
	{
		fclose(file);
		SDL_SetError("Could not load texture: file \"%s\" is empty.", filename);
		return BGFX_INVALID_HANDLE;
	}
	rewind(file);

	void* data = BX_ALLOC(&allocator_, size);
	fread(data, size, 1, file);
	fclose(file);

	bimg::ImageContainer* image = bimg::imageParse(&allocator_, data, size);
	if (!image)
	{
		free(data);
		SDL_SetError("Could not load texture: failed to parse image \"%s\".", filename);
		return BGFX_INVALID_HANDLE;
	}

	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size, ReleaseImage, image);
	BX_FREE(&allocator_, data);

	if (image->m_cubeMap)
		return bgfx::createTextureCube(static_cast<uint16_t>(image->m_width),
									   image->m_numMips > 1,
									   image->m_numLayers,
									   static_cast<bgfx::TextureFormat::Enum>(image->m_format),
									   flags,
									   mem);
	else if (image->m_depth > 1)
		return bgfx::createTexture3D(static_cast<uint16_t>(image->m_width),
									 static_cast<uint16_t>(image->m_height),
									 static_cast<uint16_t>(image->m_depth),
									 image->m_numMips > 1,
									 static_cast<bgfx::TextureFormat::Enum>(image->m_format),
									 flags,
									 mem);
	else if (bgfx::isTextureValid(0, false, image->m_numLayers, static_cast<bgfx::TextureFormat::Enum>(image->m_format), flags))
		return bgfx::createTexture2D(static_cast<uint16_t>(image->m_width),
									 static_cast<uint16_t>(image->m_height),
									 image->m_numMips > 1,
									 image->m_numLayers,
									 static_cast<bgfx::TextureFormat::Enum>(image->m_format),
									 flags,
									 mem);

	return BGFX_INVALID_HANDLE;
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
