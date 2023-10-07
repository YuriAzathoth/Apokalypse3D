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

#include <bgfx/bgfx.h>
#include <stdlib.h>
#include "Common/Camera.h"
#include "Common/Geometry.h"
#include "Common/Model.h"
#include "Common/RenderPass.h"
#include "IO/Log.h"
#include "Renderer.h"
#include "RendererAllocator.h"
#include "RendererCallback.h"

namespace A3D
{
inline static const char* GetGpuVendorName(uint16_t vendor_id);
inline static const char* GetRendererName(RendererType type);
inline static RendererType GetRendererTypeFromBgfx(bgfx::RendererType::Enum type);
inline static bgfx::RendererType::Enum GetRendererTypeToBgfx(RendererType type);
static bool SelectBestGpu(RendererGpu& gpu);

static RendererAllocator g_alloc;
static RendererCallback g_callback;
static RendererResolution g_resolution;

bool CreateRenderer(RendererGpu& gpu,
					void* window,
					void* display,
					const RendererResolution& resolution,
					RendererType type,
					uint8_t anisotropy,
					uint8_t msaa,
					bool fullscreen,
					bool vsync)
{
	LogDebug("Initialising BGFX renderer...");

	unsigned flags = BGFX_RESET_HIDPI;
	if (fullscreen) flags |= BGFX_RESET_FULLSCREEN;
	if (vsync) flags |= BGFX_RESET_VSYNC;
	switch (msaa)
	{
	case 2:  flags |= BGFX_RESET_MSAA_X2;  break;
	case 4:  flags |= BGFX_RESET_MSAA_X4;  break;
	case 8:  flags |= BGFX_RESET_MSAA_X8;  break;
	case 16: flags |= BGFX_RESET_MSAA_X16; break;
	default:; // No flags
	}

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = GetRendererTypeToBgfx(type);
	bgfxInfo.resolution.width = resolution.width;
	bgfxInfo.resolution.height = resolution.height;
	bgfxInfo.resolution.reset = flags;
	bgfxInfo.deviceId = gpu.device;
	bgfxInfo.vendorId = gpu.vendor;
	bgfxInfo.debug = BGFX_DEBUG_NONE;
	bgfxInfo.allocator = &g_alloc;
	bgfxInfo.callback = &g_callback;
	bgfxInfo.platformData.ndt = display;
	bgfxInfo.platformData.nwh = window;

	// Workaround to bgfx Caps not available before bgfx::init() called
	while (true)
	{
		if (!bgfx::init(bgfxInfo))
		{
			LogFatal("Failed to init BGFX.");
			return false;
		}

		if ((gpu.device == 0 || gpu.vendor == 0) &&
			gpu.vendor != BGFX_PCI_ID_SOFTWARE_RASTERIZER &&
			type != RendererType::OpenGL &&
			type != RendererType::Direct3D9)
		{
			LogDebug("Selecting the best GPU...");
			if (!SelectBestGpu(gpu))
				return false;

			bgfxInfo.deviceId = gpu.device;
			bgfxInfo.vendorId = gpu.vendor;
			bgfx::shutdown();
		}
		else
			break;
	}

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0 << 8 | 0xff, 1.0f, 0);

	LogInfo("BGFX Renderer initialised.");
	LogDebug("\tBackend:    %s.", GetRendererName(type));
	LogDebug("\tWidth:      %u.", resolution.width);
	LogDebug("\tHeight:     %u.", resolution.height);
	LogDebug("\tGPU vendor: %s.", GetGpuVendorName(gpu.vendor));
	LogDebug("\tGPU device: %u.", gpu.device);

	g_resolution = resolution;

	return true;
}

void DestroyRenderer()
{
	bgfx::shutdown();
	LogInfo("BGFX Renderer destroyed.");
}

void BeginRendererFrame()
{
	LogTrace("Render frame begin start...");
	bgfx::setViewRect(0, 0, 0, g_resolution.width, g_resolution.height);
	bgfx::touch(0);
	LogTrace("Render frame begin finish...");
}

void EndRendererFrame()
{
	LogTrace("Render frame end start...");
	bgfx::frame();
	LogTrace("Render frame end finish...");
}

RendererThreadContext* CreateRendererThreadContexts(uint8_t count)
{
	LogDebug("Creating %u BGFX Renderer contexts...", count);
	RendererThreadContext* ret = (RendererThreadContext*)malloc(count * sizeof(RendererThreadContext));
	if (ret != nullptr)
		LogDebug("%u BGFX Renderer contexts created.", count);
	else
		LogFatal("Failed to create %u BGFX Renderer contexts: out of memory.", count);
	return ret;
};

void DestroyRendererThreadContexts(struct RendererThreadContext* contexts)
{
	free(contexts);
	LogDebug("BGFX Renderer contexts destroyed.");
}

void BeginRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count)
{
	LogTrace("Render threads frame begin start...");

	const RendererThreadContext* end = contexts + count;
	uint8_t id = 0;
	for (RendererThreadContext* c = contexts; c < end; ++c, ++id)
	{
		c->queue = bgfx::begin(id != 0);
		Assert(c->queue != nullptr, "Renderer thread's %u queue is NULL.", id);
	}

	LogTrace("Render threads frame begin finish...");
}

void EndRendererThreadContextsFrame(RendererThreadContext* contexts, uint8_t count)
{
	LogTrace("Render threads frame end start...");

	const RendererThreadContext* end = contexts + count;
	uint8_t id = 0;
	for (RendererThreadContext* c = contexts; c < end; ++c, ++id)
	{
		Assert(c->queue != nullptr, "Renderer thread's %u queue is NULL.", id);
		bgfx::end(c->queue);
	}

	LogTrace("Render threads frame end finish...");
}

inline static const char* GetGpuVendorName(uint16_t vendor_id)
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

inline static const char* GetRendererName(RendererType type)
{
	switch (type)
	{
	case RendererType::OpenGL:		return "OpenGL";
	case RendererType::Vulkan:		return "Vulkan";
#if defined(__WIN32__)
	case RendererType::Direct3D9:	return "DirectX 9";
	case RendererType::Direct3D11:	return "DirectX 11";
	case RendererType::Direct3D12:	return "DirectX 12";
#elif defined(OSX)
	case RendererType::Metal:		return "Metal";
#endif // defined
	case RendererType::None:		return "None";
	default:						return "Automatic";
	}
}

inline static RendererType GetRendererTypeFromBgfx(bgfx::RendererType::Enum type)
{
	switch (type)
	{
	case bgfx::RendererType::Count:		return RendererType::Auto;
	case bgfx::RendererType::OpenGL:	return RendererType::OpenGL;
	case bgfx::RendererType::Vulkan:	return RendererType::Vulkan;
#if defined(__WIN32__)
	case bgfx::RendererType::Direct3D9:	return RendererType::Direct3D9;
	case bgfx::RendererType::Direct3D11:return RendererType::Direct3D11;
	case bgfx::RendererType::Direct3D12:return RendererType::Direct3D12;
#elif defined(OSX)
	case bgfx::RendererType::Metal:		return RendererType::Metal;
#endif // defined
	case bgfx::RendererType::Noop:		return RendererType::None;
	default:							return RendererType::Auto;
	}
}

inline static bgfx::RendererType::Enum GetRendererTypeToBgfx(RendererType type)
{
	switch (type)
	{
	case RendererType::Auto:		return bgfx::RendererType::Count;
	case RendererType::OpenGL:		return bgfx::RendererType::OpenGL;
	case RendererType::Vulkan:		return bgfx::RendererType::Vulkan;
#if defined(__WIN32__)
	case RendererType::Direct3D9:	return bgfx::RendererType::Direct3D9;
	case RendererType::Direct3D11:	return bgfx::RendererType::Direct3D11;
	case RendererType::Direct3D12:	return bgfx::RendererType::Direct3D12;
#elif defined(OSX)
	case RendererType::Metal:		return bgfx::RendererType::Metal;
#endif // defined
	case RendererType::None:		return bgfx::RendererType::Noop;
	default:						return bgfx::RendererType::Count;
	}
}

static bool SelectBestGpu(RendererGpu& gpu)
{
	const bgfx::Caps* caps = bgfx::getCaps();
	if (caps == nullptr)
	{
		LogError("Could not select best GPU: failed to get BGFX caps.");
		return false;
	}
	if (caps->numGPUs < 1)
	{
		LogError("Could not find any installed GPUs in system.");
		return false;
	}

	LogInfo("Found GPU devices in system:");
	for (unsigned i = 0; i < caps->numGPUs; ++i)
		LogInfo("\t%s", GetGpuVendorName(caps->gpu[i].vendorId));

	uint16_t curr_vendor_id;
	for (unsigned i = 0; i < caps->numGPUs; ++i)
	{
		curr_vendor_id = caps->gpu[i].vendorId;
		if (curr_vendor_id != BGFX_PCI_ID_NONE)
		{
			gpu.device = caps->gpu[i].deviceId;
			gpu.vendor = curr_vendor_id;
			if (gpu.vendor == BGFX_PCI_ID_NVIDIA || gpu.vendor == BGFX_PCI_ID_AMD)
				break;
		}
	}
	return true;
}

void SetCameraTransforms(const Camera* cameras, uint16_t count)
{
	for (uint16_t i = 0; i < count; ++i, ++cameras)
		bgfx::setViewTransform(i, cameras->view, cameras->proj);
}

void DrawMeshGroup(const MeshGroup& mesh, const RenderPass& pass, const GlobalTransform& transform)
{
	bgfx::setTransform(transform.transform);
	bgfx::setVertexBuffer(0, mesh.vbo);
	bgfx::setIndexBuffer(mesh.ebo);
	bgfx::setState(BGFX_STATE_DEFAULT);
	bgfx::submit(0, pass.program);
}

void DrawMeshGroup(RendererThreadContext& context,
				   const MeshGroup& mesh,
				   const RenderPass& pass,
				   const GlobalTransform& transform)
{
	context.queue->setTransform(transform.transform);
	context.queue->setVertexBuffer(0, mesh.vbo);
	context.queue->setIndexBuffer(mesh.ebo);
	context.queue->setState(BGFX_STATE_DEFAULT);
	context.queue->submit(0, pass.program);
}
} // namespace A3D
