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

#include <bgfx/bgfx.h>
#include <stdlib.h>
//#include "Common/Camera.h"
//#include "Common/Geometry.h"
//#include "Common/Material.h"
//#include "Common/Model.h"
#include "Common/Config.h"
#include "Core/ILog.h"
//#include "Resource/Material.h"
#include "Renderer.h"
#include "RendererAllocator.h"
#include "RendererCallback.h"

static const char SUFFIX_GL[] = "../Data/Shaders/gl";
static const char SUFFIX_GLES[] = "../Data/Shaders/gles";
static const char SUFFIX_VK[] = "../Data/Shaders/vk";
static const char SUFFIX_DX_11[] = "../Data/Shaders/dx11";
static const char SUFFIX_METAL[] = "../Data/Shaders/metal";
namespace A3D
{
static const char* GetGpuVendorName(uint16_t vendor_id);
static const char* GetRendererName(RendererType type);
static RendererType GetRendererTypeFromBgfx(bgfx::RendererType::Enum type);
static bgfx::RendererType::Enum GetRendererTypeToBgfx(RendererType type);

bool Renderer::s_initialized = false;

Renderer::Renderer(ILog* log, bx::AllocatorI* alloc, bgfx::CallbackI* callback) :
	resolution_{},
	log_(log),
	alloc_(alloc),
	callback_(callback)
{
}

Renderer::~Renderer()
{
	if (s_initialized == true)
		Shutdown();
}

bool Renderer::Initialize(GraphicsConfig& config, WindowPlatformData& wpd)
{
	if (s_initialized == true)
	{
		log_->Fatal("Could not initialize renderer system: renderer is already initialized.");
		return false;
	}

	log_->Debug("Initialising BGFX renderer...");

	unsigned flags = BGFX_RESET_HIDPI;
	if (config.fullscreen) flags |= BGFX_RESET_FULLSCREEN;
	if (config.vsync) flags |= BGFX_RESET_VSYNC;
	switch (config.msaa)
	{
	case 2:  flags |= BGFX_RESET_MSAA_X2;  break;
	case 4:  flags |= BGFX_RESET_MSAA_X4;  break;
	case 8:  flags |= BGFX_RESET_MSAA_X8;  break;
	case 16: flags |= BGFX_RESET_MSAA_X16; break;
	default:; // No flags
	}

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = GetRendererTypeToBgfx(config.renderer);
	bgfxInfo.resolution.width = config.resolution.width;
	bgfxInfo.resolution.height = config.resolution.height;
	bgfxInfo.resolution.reset = flags;
	bgfxInfo.deviceId = config.gpu.device;
	bgfxInfo.vendorId = config.gpu.vendor;
	bgfxInfo.debug = BGFX_DEBUG_NONE;
	bgfxInfo.allocator = alloc_;
	bgfxInfo.callback = callback_;
	bgfxInfo.platformData.ndt = wpd.display;
	bgfxInfo.platformData.nwh = wpd.window;

	resolution_.width = config.resolution.width;
	resolution_.height = config.resolution.height;

	// Workaround to bgfx Caps not available before bgfx::init() called.
	while (true)
	{
		if (!bgfx::init(bgfxInfo))
		{
			log_->Fatal("Failed to init BGFX.");
			return false;
		}

		if ((config.gpu.device == 0 || config.gpu.vendor == 0) &&
			config.gpu.vendor != BGFX_PCI_ID_SOFTWARE_RASTERIZER &&
			config.renderer != RendererType::OpenGL)
		{
			log_->Info("Selecting the best GPU...");
			if (!SelectBestGpu(config))
				return false;

			bgfxInfo.deviceId = config.gpu.device;
			bgfxInfo.vendorId = config.gpu.vendor;
			bgfx::shutdown();
		}
		else
			break;
	}

	if (config.threads != thread_encoders_.size())
	{
		thread_encoders_.clear();
		for (int i = 0; i < config.threads; ++i)
			thread_encoders_.emplace_back();
		/*if (config.threads == 1)
			thread_encoders_.clear();
		else
			thread_encoders_.resize(count);*/
	}

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0 << 8 | 0xff, 1.0f, 0);

	bgfx::RendererType::Enum renderer = bgfx::getRendererType();
	switch (renderer)
	{
	case bgfx::RendererType::OpenGL:		shader_prefix_ = SUFFIX_GL; break;
	case bgfx::RendererType::OpenGLES:		shader_prefix_ = SUFFIX_GLES; break;
	case bgfx::RendererType::Vulkan:		shader_prefix_ = SUFFIX_VK; break;
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12:	shader_prefix_ = SUFFIX_DX_11; break;
	case bgfx::RendererType::Metal:			shader_prefix_ = SUFFIX_METAL; break;
	default:;
	}

	log_->Info("BGFX Renderer initialised.");
	log_->Debug("\tBackend:    %s.", GetRendererName(GetRendererTypeFromBgfx(renderer)));
	log_->Debug("\tWidth:      %u.", config.resolution.width);
	log_->Debug("\tHeight:     %u.", config.resolution.height);
	log_->Debug("\tGPU vendor: %s.", GetGpuVendorName(config.gpu.vendor));
	log_->Debug("\tGPU device: %u.", config.gpu.device);

	s_initialized = true;

	return true;
}

void Renderer::Shutdown()
{
	if (s_initialized == false)
	{
		log_->Fatal("Could not shutdown renderer system: renderer is not initialized yet.");
		return;
	}

	s_initialized = false;

	bgfx::shutdown();

	log_->Info("BGFX Renderer destroyed.");
}

void Renderer::BeginFrame()
{
	log_->Trace("Render frame begin started...");

	bgfx::setViewRect(0, 0, 0, resolution_.width, resolution_.height);
	bgfx::touch(0);

	log_->Trace("Render frame begin finished...");

	if (!thread_encoders_.empty())
	{
		uint8_t id = 0;
		for (auto it = thread_encoders_.begin(); it != thread_encoders_.end(); ++it, ++id)
		{
			*it = bgfx::begin(id != 0);
#ifndef NDEBUG
			if (*it != nullptr)
				log_->Fatal("Renderer thread's %u queue is NULL.", id);
#endif // NDEBUG
		}

		log_->Trace("Render frame begin threads finished...");
	}
}

void Renderer::EndFrame()
{
	log_->Trace("Render frame end started...");

	if (!thread_encoders_.empty())
	{
		uint8_t id = 0;
		for (auto it = thread_encoders_.begin(); it != thread_encoders_.end(); ++it, ++id)
		{
#ifndef NDEBUG
			if (*it != nullptr)
				log_->Fatal("Renderer thread's %u queue is NULL.", id);
#endif // NDEBUG
			bgfx::end(*it);
		}

		log_->Trace("Render frame end threads finished...");
	}

	bgfx::frame();

	log_->Trace("Render frame end finished...");
}

bool Renderer::SelectBestGpu(GraphicsConfig& config)
{
	const bgfx::Caps* caps = bgfx::getCaps();
	if (caps == nullptr)
	{
		log_->Error("Could not select best GPU: failed to get BGFX caps.");
		return false;
	}
	if (caps->numGPUs < 1)
	{
		log_->Error("Could not select best GPU: failed to find any installed GPUs in system.");
		return false;
	}

#ifndef NDEBUG
	log_->Info("Found GPU devices in system:");
	for (unsigned i = 0; i < caps->numGPUs; ++i)
		log_->Info("\t%s", GetGpuVendorName(caps->gpu[i].vendorId));
#endif // NDEBUG

	uint16_t curr_vendor_id;
	for (unsigned i = 0; i < caps->numGPUs; ++i)
	{
		curr_vendor_id = caps->gpu[i].vendorId;
		if (curr_vendor_id != BGFX_PCI_ID_NONE)
		{
			config.gpu.device = caps->gpu[i].deviceId;
			config.gpu.vendor = curr_vendor_id;
			if (config.gpu.vendor == BGFX_PCI_ID_NVIDIA || config.gpu.vendor == BGFX_PCI_ID_AMD)
				break;
		}
	}
	return true;
}

/*void SetCameraTransforms(const Camera* cameras, uint16_t count)
{
	for (uint16_t i = 0; i < count; ++i, ++cameras)
		bgfx::setViewTransform(i, cameras->view, cameras->proj);
}

void DrawMeshGroup(const MeshGroup& mesh, const Material& material, const GlobalTransform& transform)
{
	bgfx::setTransform(transform.transform);
	bgfx::setVertexBuffer(0, mesh.vbo);
	bgfx::setIndexBuffer(mesh.ebo);
	bgfx::setState(BGFX_STATE_DEFAULT);

	bgfx::ProgramHandle program = UseMaterial(material);
	bgfx::submit(0, program);
}

void DrawMeshGroup(RendererThreadContext& context,
				   const MeshGroup& mesh,
				   const Material& material,
				   const GlobalTransform& transform)
{
	context.queue->setTransform(transform.transform);
	context.queue->setVertexBuffer(0, mesh.vbo);
	context.queue->setIndexBuffer(mesh.ebo);
	context.queue->setState(BGFX_STATE_DEFAULT);

	bgfx::ProgramHandle program = UseMaterial(material, context.queue);
	context.queue->submit(0, program);
}*/

static const char* GetGpuVendorName(uint16_t vendor_id)
{
	switch (vendor_id)
	{
	case BGFX_PCI_ID_AMD:					return "AMD";
	case BGFX_PCI_ID_APPLE:					return "Apple";
	case BGFX_PCI_ID_ARM:					return "Arm";
	case BGFX_PCI_ID_INTEL:					return "Intel";
	case BGFX_PCI_ID_MICROSOFT:				return "Microsoft";
	case BGFX_PCI_ID_NVIDIA:				return "NVidia";
	case BGFX_PCI_ID_SOFTWARE_RASTERIZER:	return "Software";
	default:								return "None";
	}
}

static const char* GetRendererName(RendererType type)
{
	switch (type)
	{
	case RendererType::OpenGL:		return "OpenGL";
	case RendererType::Vulkan:		return "Vulkan";
#if defined(__WIN32__)
	case RendererType::Direct3D11:	return "DirectX 11";
	case RendererType::Direct3D12:	return "DirectX 12";
#elif defined(OSX)
	case RendererType::Metal:		return "Metal";
#endif // defined
	case RendererType::None:		return "None";
	default:						return "Automatic";
	}
}

static RendererType GetRendererTypeFromBgfx(bgfx::RendererType::Enum type)
{
	switch (type)
	{
	case bgfx::RendererType::Count:		return RendererType::Auto;
	case bgfx::RendererType::OpenGL:	return RendererType::OpenGL;
	case bgfx::RendererType::Vulkan:	return RendererType::Vulkan;
#if defined(__WIN32__)
	case bgfx::RendererType::Direct3D11:return RendererType::Direct3D11;
	case bgfx::RendererType::Direct3D12:return RendererType::Direct3D12;
#elif defined(OSX)
	case bgfx::RendererType::Metal:		return RendererType::Metal;
#endif // defined
	case bgfx::RendererType::Noop:		return RendererType::None;
	default:							return RendererType::Auto;
	}
}

static bgfx::RendererType::Enum GetRendererTypeToBgfx(RendererType type)
{
	switch (type)
	{
	case RendererType::Auto:		return bgfx::RendererType::Count;
	case RendererType::OpenGL:		return bgfx::RendererType::OpenGL;
	case RendererType::Vulkan:		return bgfx::RendererType::Vulkan;
#if defined(__WIN32__)
	case RendererType::Direct3D11:	return bgfx::RendererType::Direct3D11;
	case RendererType::Direct3D12:	return bgfx::RendererType::Direct3D12;
#elif defined(OSX)
	case RendererType::Metal:		return bgfx::RendererType::Metal;
#endif // defined
	case RendererType::None:		return bgfx::RendererType::Noop;
	default:						return bgfx::RendererType::Count;
	}
}
} // namespace A3D
