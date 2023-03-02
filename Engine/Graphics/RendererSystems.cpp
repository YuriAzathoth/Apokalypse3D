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
#include <glm/gtc/type_ptr.hpp>
#include <SDL3/SDL.h>
#include "CameraComponents.h"
#include "GraphicsComponents.h"
#include "GraphicsPlatform.h"
#include "IO/Log.h"
#include "RendererAllocator.h"
#include "RendererCallback.h"
#include "RendererComponents.h"
#include "RendererSystems.h"
#include "WindowComponents.h"

using namespace A3D::Components::Camera;
using namespace A3D::Components::Graphics;
using namespace A3D::Components::Renderer;
using namespace A3D::Components::Window;

static A3D::RendererAllocator g_alloc;
static A3D::RendererCallback g_callback;

inline static const char* get_renderer_type_name(RendererType type) noexcept
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

inline static const char* get_gpu_vendor_name(uint16_t vendor_id) noexcept
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

inline static unsigned parse_flags(MsaaLevel msaa, WindowMode mode, bool vsync) noexcept
{
	unsigned flags = BGFX_RESET_HIDPI;
	if (mode == WindowMode::Fullscreen) flags |= BGFX_RESET_FULLSCREEN;
	if (vsync) flags |= BGFX_RESET_VSYNC;
	switch (msaa)
	{
	case MsaaLevel::X2:  flags |= BGFX_RESET_MSAA_X2;  break;
	case MsaaLevel::X4:  flags |= BGFX_RESET_MSAA_X4;  break;
	case MsaaLevel::X8:  flags |= BGFX_RESET_MSAA_X8;  break;
	case MsaaLevel::X16: flags |= BGFX_RESET_MSAA_X16; break;
	default:; // No flags on no MSAA
	}
	return flags;
}

inline static RendererType renderer_type_from_bgfx(bgfx::RendererType::Enum type) noexcept
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

inline static bgfx::RendererType::Enum renderer_type_to_bgfx(RendererType type) noexcept
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

static bool select_best_gpu(uint16_t& device_id, uint16_t& vendor_id) noexcept
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
		LogInfo("\t%s", get_gpu_vendor_name(caps->gpu[i].vendorId));

	uint16_t curr_vendor_id;
	for (unsigned i = 0; i < caps->numGPUs; ++i)
	{
		curr_vendor_id = caps->gpu[i].vendorId;
		if (curr_vendor_id != BGFX_PCI_ID_NONE)
		{
			device_id = caps->gpu[i].deviceId;
			vendor_id = curr_vendor_id;
			if (vendor_id == BGFX_PCI_ID_NVIDIA || vendor_id == BGFX_PCI_ID_AMD)
				break;
		}
	}
	return true;
}

static void init_default(flecs::iter it,
						 size_t,
						 const Device* device,
						 const MsaaLevel* msaa,
						 const RendererType* type)
{
	LogDebug("Initializing BGFX renderer default config...");
	using Startup = A3D::Components::Renderer::Startup;
	flecs::world w = it.world();
	w.add<Startup>();
	if (device == nullptr)
		w.set<Device>({0, 0});
	if (msaa == nullptr)
		w.set<MsaaLevel>(MsaaLevel::NONE);
	if (type == nullptr)
		w.set<RendererType>(RendererType::Auto);
}

static void init_renderer(flecs::iter it,
						  size_t,
						  Device& device,
						  const Window& window,
						  const Resolution& resolution,
						  const MsaaLevel msaa,
						  const RendererType type,
						  const WindowMode mode)
{
	LogDebug("Initialising BGFX renderer...");

	using Startup = A3D::Components::Renderer::Startup;
	flecs::world w = it.world();

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = renderer_type_to_bgfx(type);
	bgfxInfo.resolution.width = resolution.width;
	bgfxInfo.resolution.height = resolution.height;
	bgfxInfo.resolution.reset = parse_flags(msaa, mode, w.has<VSync>());
	bgfxInfo.deviceId = device.device;
	bgfxInfo.vendorId = device.vendor;
	bgfxInfo.debug = BGFX_DEBUG_NONE;
	bgfxInfo.allocator = &g_alloc;
	bgfxInfo.callback = &g_callback;
	if (!bind_bgfx_to_sdl(bgfxInfo.platformData, window.window))
	{
		LogError("Could not attach bgfx renderer to SDL: %s.", SDL_GetError());
		w.quit();
		return;
	}

	while (true)
	{
		if (!bgfx::init(bgfxInfo))
		{
			LogError("Failed to init BGFX.");
			w.quit();
			return;
		}
		if ((device.device == 0 || device.vendor == 0) &&
			device.vendor != BGFX_PCI_ID_SOFTWARE_RASTERIZER &&
			type != RendererType::OpenGL &&
			type != RendererType::Direct3D9)
		{
			LogDebug("Selecting the best GPU...");
			if (!select_best_gpu(device.device, device.vendor))
			{
				w.quit();
				return;
			}
			bgfxInfo.deviceId = device.device;
			bgfxInfo.vendorId = device.vendor;
			bgfx::shutdown();
		}
		else
			break;
	}

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0 << 8 | 0xff, 1.0f, 0);

	w.add<Renderer>();

	LogInfo("BGFX Renderer initialised.");
	LogInfo("  Renderer backend:    %s.", get_renderer_type_name(type));
	LogInfo("  Renderer width:      %u.", resolution.width);
	LogInfo("  Renderer height:     %u.", resolution.height);
	LogInfo("  Renderer GPU vendor: %s.", get_gpu_vendor_name(device.vendor));
	LogInfo("  Renderer GPU device: %u.", device.device);

	w.remove<Startup>();
}

static void destroy_renderer(flecs::entity e) { bgfx::shutdown(); }

static void frame_begin(const Resolution& resolution)
{
	LogTrace("Render frame begin start...");

	bgfx::setViewRect(0, 0, 0, resolution.width, resolution.height);
	bgfx::touch(0);

	LogTrace("Render frame begin finish...");
}

static void frame_end(flecs::iter&, size_t)
{
	LogTrace("Render frame end start...");

	bgfx::frame();

	LogTrace("Render frame end finish...");
}

static void threads_begin(flecs::iter& it, size_t, const RendererThreads& threads)
{
	LogTrace("Render threads begin start...");

	flecs::world w = it.world();
	for (unsigned i = 0; i < w.get_threads(); ++i)
	{
		threads.queues[i] = bgfx::begin(i != 0);
		Assert(threads.queues[i], "Renderer threads' queue is NULL.");
	}

	LogTrace("Render threads begin finish...");
}

static void threads_end(flecs::iter& it, size_t, const RendererThreads& threads)
{
	LogTrace("Render threads end start...");

	flecs::world w = it.world();
	for (unsigned i = 0; i < w.get_threads(); ++i)
	{
		Assert(threads.queues[i], "Renderer threads' queue is NULL.");
		bgfx::end(threads.queues[i]);
	}

	LogTrace("Render threads end finish...");
}

static void update_camera(const Eye& eye)
{
	LogTrace("Send camera transform...");
	bgfx::setViewTransform(0, glm::value_ptr(eye.view), glm::value_ptr(eye.proj));
}

static void update_aspect(flecs::iter& it, size_t, const Resolution& resolution)
{
	it.world().set<Aspect>({(float)resolution.width / (float)resolution.height});
}

static void add_threads(flecs::iter& it, size_t)
{
	flecs::world w = it.world();
	RendererThreads threads;
	threads.count = w.get_stage_count();
	threads.queues = (bgfx::Encoder**)malloc(threads.count * sizeof(bgfx::Encoder*));
	w.set<RendererThreads>(threads);
}

static void remove_threads(flecs::iter& it, size_t)
{
	it.world().remove<RendererThreads>();
}

A3D::RendererSystems::RendererSystems(flecs::world& world)
{
	using Startup = A3D::Components::Renderer::Startup;

	world.module<RendererSystems>("A3D::Systems::Renderer");
	world.import<CameraComponents>();
	world.import<GraphicsComponents>();
	world.import<RendererComponents>();
	world.import<WindowComponents>();

	initDefault_ = world.system<const Device*, const MsaaLevel*, const RendererType*>("InitDefault")
		.arg(1).singleton()
		.arg(2).singleton()
		.arg(3).singleton()
		.kind(flecs::OnStart)
		.each(init_default);

	initRenderer_ = world.system<Device,
								 const Window,
								 const Resolution,
								 const MsaaLevel,
								 const RendererType,
								 const WindowMode>("InitRenderer")
		.arg(1).singleton()
		.arg(2).singleton()
		.arg(3).singleton()
		.arg(4).singleton()
		.arg(5).singleton()
		.arg(6).singleton()
		.with<Startup>().singleton()
		.kind(flecs::OnLoad)
		.no_readonly()
		.each(init_renderer);

	destroy_ = world.observer<>("DestroyRenderer")
		.term<Renderer>().singleton()
		.event(flecs::UnSet)
		.each(destroy_renderer);

	frameBegin_ = world.system<const Resolution>("FrameBegin")
				  .arg(1).singleton()
				  .with<Renderer>().singleton()
				  .kind(flecs::PostUpdate)
				  .each(frame_begin);

	threadsBegin_ = world.system<const RendererThreads>("ThreadsBegin")
					.arg(1).singleton()
					.term<const Resolution>().singleton()
					.with<MultiThreaded>().singleton()
					.with<Renderer>().singleton()
					.kind(flecs::PostUpdate)
					.each(threads_begin);

	threadsEnd_ = world.system<const RendererThreads>("ThreadsEnd")
				  .arg(1).singleton()
				  .term<const Resolution>().singleton()
				  .with<MultiThreaded>().singleton()
				  .with<Renderer>().singleton()
				  .kind(flecs::OnStore)
				  .each(threads_end);

	frameEnd_ = world.system<>("FrameEnd")
				.term<const Resolution>().singleton()
				.with<Renderer>().singleton()
				.kind(flecs::OnStore)
				.each(frame_end);

	updateCameraView_ = world.system<const Eye>("UpdateCameraView")
						.term<const Renderer>().singleton()
						.kind(flecs::OnStore)
						.each(update_camera);

	updateAspect_ = world.observer<const Resolution>("UpdateAspect")
					.arg(1).singleton()
					.event(flecs::OnSet)
					.each(update_aspect);

	addThreads_ = world.observer<>("AddThreads")
					.term<MultiThreaded>().singleton()
					.event(flecs::OnSet)
					.each(add_threads);

	removeThreads_ = world.observer<>("RemoveThreads")
					.term<MultiThreaded>().singleton()
					.event(flecs::OnRemove)
					.each(remove_threads);
}
