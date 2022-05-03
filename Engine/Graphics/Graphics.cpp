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

#define EASTL_DLL 1

#include <EASTL/set.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <bimg/decode.h>
#include <bgfx/platform.h>
#include <flecs.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include "Core/Platform.h"
#include "Graphics.h"
#include "GraphicsComponents.h"
#include "IO/Log.h"
#include "Scene/SceneComponents.h"

#if BX_PLATFORM_EMSCRIPTEN
#include "emscripten.h"
#endif // BX_PLATFORM_EMSCRIPTEN

Graphics::Graphics(const InitInfo& initInfo, bool& initialized)
	: window_(nullptr)
{
	initialized = false;

	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		LogError("Failed to init SDL video subsystem: %s", SDL_GetError());
		return;
	}

	windowWidth_ = initInfo.width;
	windowHeight_ = initInfo.height;
	if (!initInfo.width || !initInfo.height)
	{
		const int modes = SDL_GetNumDisplayModes(initInfo.display);
		SDL_DisplayMode mode;
		for (int i = 0; i < modes; ++i)
		{
			SDL_GetDisplayMode(initInfo.display, i, &mode);
			if (windowWidth_ < mode.w || windowHeight_ < mode.h)
			{
				windowWidth_ = mode.w;
				windowHeight_ = mode.h;
			}
		}
	}

	unsigned sdlFlags = 0;
	unsigned bgfxFlags = 0;
	if (initInfo.fullscreen)
	{
		bgfxFlags |= BGFX_RESET_FULLSCREEN;
		sdlFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
	}
	if (initInfo.highDpi)
	{
		bgfxFlags |= BGFX_RESET_HIDPI;
		sdlFlags |= SDL_WINDOW_ALLOW_HIGHDPI;
		DisableHighDpi();
	}
	if (initInfo.vsync)
		bgfxFlags |= BGFX_RESET_VSYNC;

	constexpr unsigned MSAA_FLAGS[] =
	{
		0,
		BGFX_RESET_MSAA_X2,
		BGFX_RESET_MSAA_X4,
		BGFX_RESET_MSAA_X8,
		BGFX_RESET_MSAA_X16,
	};
	bgfxFlags |= MSAA_FLAGS[static_cast<unsigned>(initInfo.msaa)];

	window_ = SDL_CreateWindow(initInfo.title,
							   SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED,
							   windowWidth_,
							   windowHeight_,
							   SDL_WINDOW_SHOWN | sdlFlags);
	if (!window_)
	{
		LogError("Failed to create SDL window: %s", SDL_GetError());
		return;
	}

#if !BX_PLATFORM_EMSCRIPTEN
	SDL_SysWMinfo wmi;
	SDL_VERSION(&wmi.version);
	if (!SDL_GetWindowWMInfo(window_, &wmi))
	{
		LogError("Could now get SDL SysWM info: %s", SDL_GetError());
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

	/*unsigned short gpuVendor;
	{
		struct GpuComparator
		{
			inline unsigned GetWeight(const bgfx::Caps::GPU* gpu) const noexcept
			{
				switch (gpu->vendorId)
				{
				case BGFX_PCI_ID_NVIDIA:
					return 2;
				case BGFX_PCI_ID_AMD:
					return 1;
				default:
					return 0;
				}
			}
			inline bool operator()(const bgfx::Caps::GPU* lhs, const bgfx::Caps::GPU* rhs) const noexcept
			{
				return GetWeight(lhs) < GetWeight(rhs);
			}
		};
		eastl::multiset<const bgfx::Caps::GPU*, GpuComparator> gpus;
		for (const bgfx::Caps::GPU* gpuptr = bgfx::getCaps()->gpu;
			 gpuptr < bgfx::getCaps()->gpu + bgfx::getCaps()->numGPUs;
			 ++gpuptr)
			gpus.insert(gpuptr);
		gpuVendor = (*gpus.begin())->vendorId;
	}*/

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = static_cast<bgfx::RendererType::Enum>(initInfo.render);
	bgfxInfo.vendorId = BGFX_PCI_ID_NVIDIA; //gpuVendor;
	bgfxInfo.resolution.width = windowWidth_;
	bgfxInfo.resolution.height = windowHeight_;
	bgfxInfo.resolution.reset = bgfxFlags;
	if (!bgfx::init(bgfxInfo))
	{
		LogError("Failed to init BGFX");
		return;
	}
	aspectRatio_ = static_cast<float>(windowWidth_) / static_cast<float>(windowHeight_);

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
	while (bgfx::RenderFrame::NoContext != bgfx::renderFrame())
		;
	bgfx::shutdown();
	SDL_DestroyWindow(window_);
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Graphics::InitSystems(flecs::world& world)
{
	world.system<const Camera>().each(
		[](const Camera& camera)
		{ bgfx::setViewTransform(0, glm::value_ptr(camera.view), glm::value_ptr(camera.proj)); });

	world.system<Perspective, const WindowAspect>().kind(flecs::PreUpdate).each(
		[this](Perspective& perspective, const WindowAspect&) {
			perspective.aspect = GetWindowAspect();
		});

	world.system<Camera, const Perspective>().kind(flecs::PreUpdate).each(
		[](Camera& camera, const Perspective& perspective) {
			camera.proj = glm::perspective(perspective.fov,
										   perspective.aspect,
										   perspective.nearest,
										   perspective.farthest);
		});

	world.system<Camera, const Ray>().kind(flecs::PreUpdate).each(
		[](Camera& camera, const Ray& ray) {
			const glm::vec3 forward = glm::normalize(ray.end - ray.begin);
			const glm::vec3 up = glm::angleAxis(glm::radians(-90.0f), glm::vec3{1.0f, 0.0f, 0.0f}) * forward;
			camera.view = glm::lookAt(ray.begin, ray.end, up);
		});
}

void Graphics::BeginFrame() noexcept
{
	bgfx::setViewRect(0, 0, 0, windowWidth_, windowHeight_);
	bgfx::touch(0);
}

void Graphics::EndFrame() noexcept { bgfx::frame(); }

void Graphics::DrawScene(flecs::entity node) const noexcept
{
	const Model* model = node.get<Model>();
	const MaterialBasic* material = node.get<MaterialBasic>();
	const Node* cnode = node.get<Node>();
	if (model && material && cnode)
	{
		bgfx::setTransform(glm::value_ptr(cnode->model));
		bgfx::setVertexBuffer(0, model->vbo);
		bgfx::setIndexBuffer(model->ebo);
		bgfx::setState(BGFX_STATE_DEFAULT);
		bgfx::setTexture(0, GetUniformHandle("s_texColor"), material->diffuse);
		bgfx::submit(0, material->program);
	}
	node.children([this](flecs::entity child) { DrawScene(child); });
}

void Graphics::SetTexture(bgfx::TextureHandle texture, unsigned short slot) const noexcept
{
	bgfx::setTexture(slot, GetUniformHandle("s_texColor"), texture);
}

bgfx::ShaderHandle Graphics::LoadShader(const char* filename) noexcept
{
	const char* subpath;
	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Direct3D9:
		subpath = "dx9";
		break;
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12:
		subpath = "dx11";
		break;
	case bgfx::RendererType::Metal:
		subpath = "msl";
		break;
	case bgfx::RendererType::OpenGLES:
		subpath = "essl";
		break;
	case bgfx::RendererType::OpenGL:
		subpath = "glsl";
		break;
	case bgfx::RendererType::Vulkan:
		subpath = "spirv";
		break;
	default:
		return BGFX_INVALID_HANDLE;
	}

	char filepath[256];
	sprintf(filepath, "../Data/Shaders/%s/%s", subpath, filename);
	FILE* file = fopen(filepath, "rb");
	if (!file)
	{
		LogError("Could not create shader: file \"%s\" not found.", filename);
		return BGFX_INVALID_HANDLE;
	}

	fseek(file, 0, SEEK_END);
	const unsigned size = static_cast<unsigned>(ftell(file));
	if (!size)
	{
		fclose(file);
		LogError("Could not create shader: file \"%s\" is empty.", filename);
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
	return bgfx::createProgram(vertexShader, fragmentShader, false);
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
		LogError("Could not load texture \"%s\": file not found.", filename);
		return BGFX_INVALID_HANDLE;
	}

	fseek(file, 0, SEEK_END);
	const unsigned size = static_cast<unsigned>(ftell(file));
	if (!size)
	{
		fclose(file);
		LogError("Could not load texture \"%s\": file is empty.", filename);
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
		LogError("Could not load texture \"%s\": failed to parse image.", filename);
		return BGFX_INVALID_HANDLE;
	}

	const bgfx::Memory* mem = bgfx::makeRef(image->m_data, image->m_size, [](void*, void* data)
	{
		bimg::imageFree(reinterpret_cast<bimg::ImageContainer*>(data));
	}, image);
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

	LogError("Could not load texture \"%s\": unknown texture type.", filename);
	return BGFX_INVALID_HANDLE;
}

eastl::vector<Graphics::WindowMode> Graphics::GetWindowResolutions() const noexcept
{
	const int display = SDL_GetWindowDisplayIndex(window_);
	const int modes = SDL_GetNumDisplayModes(display);
	eastl::vector<Graphics::WindowMode> ret;
	ret.reserve(modes);
	SDL_DisplayMode mode;
	for (int i = 0; i < modes; ++i)
	{
		SDL_GetDisplayMode(display, i, &mode);
		ret.push_back({mode.w, mode.h, mode.refresh_rate});
	}
	return ret;
}
