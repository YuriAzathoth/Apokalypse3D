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

#include <bgfx/bgfx.h>
#include <SDL3/SDL.h>
#include "IO/Log.h"
#include "GraphicsPlatform.h"
#include "RendererComponents.h"
#include "RendererSystems.h"
#include "WindowComponents.h"

using namespace A3D::Components::Renderer;
using namespace A3D::Components::Window;

namespace A3D
{
using Renderer = A3D::Components::Renderer::Renderer;

static RendererAllocator g_alloc;
static RendererCallback g_callback;

inline static const char* get_gpu_vendor_name(uint16_t vendor_id) noexcept;
inline static const char* get_renderer_type_name(RendererType type) noexcept;
inline static bool normalize_config(RendererConfig& config, SDL_Window* window, const flecs::world& world) noexcept;
inline static unsigned parse_flags(const RendererConfig& config) noexcept;
inline static RendererType renderer_type_from_bgfx(bgfx::RendererType::Enum type) noexcept;
inline static bgfx::RendererType::Enum renderer_type_to_bgfx(RendererType type) noexcept;
inline static bool select_best_gpu(uint16_t& device_id, uint16_t& vendor_id) noexcept;

bool create_renderer(flecs::world& world)
{
	return create_renderer(world, *world.get_mut<RendererConfig>());
}

bool create_renderer(flecs::world& world, Components::Renderer::RendererConfig& config)
{
	LogDebug("Initialising BGFX Renderer...");

	const Window* window = world.get<Window>();
	ecs_assert_var(window->window, -1, "SDL window must be created before BGFX initialization.");

	if (normalize_config(config, window->window, world))
		world.modified<RendererConfig>();

	LogInfo("Renderer backend:    %s.", get_renderer_type_name(config.type));
	LogInfo("Renderer width:      %u.", config.width);
	LogInfo("Renderer height:     %u.", config.height);

	bgfx::Init bgfxInfo{};
	bgfxInfo.type = renderer_type_to_bgfx(config.type);
	bgfxInfo.resolution.width = config.width;
	bgfxInfo.resolution.height = config.height;
	bgfxInfo.resolution.reset = parse_flags(config);
	bgfxInfo.deviceId = config.deviceId;
	bgfxInfo.vendorId = config.vendorId;
	bgfxInfo.debug = BGFX_DEBUG_NONE;
	bgfxInfo.allocator = &g_alloc;
	bgfxInfo.callback = &g_callback;
	if (!bind_bgfx_to_sdl(bgfxInfo.platformData, window->window))
	{
		LogError("Could not attach bgfx renderer to SDL: %s.", SDL_GetError());
		world.quit();
		return false;
	}

	while (true)
	{
		if (!bgfx::init(bgfxInfo))
		{
			LogError("Failed to init BGFX.");
			world.quit();
			return false;
		}
		if ((config.deviceId == 0 || config.vendorId == 0) &&
			config.vendorId != BGFX_PCI_ID_SOFTWARE_RASTERIZER &&
			config.type != RendererType::OpenGL &&
			config.type != RendererType::Direct3D9)
		{
			LogDebug("Selecting the best GPU...");
			if (!select_best_gpu(config.deviceId, config.vendorId))
			{
				world.quit();
				return false;
			}
			else
			{
				LogInfo("Renderer GPU vendor: %s.", get_gpu_vendor_name(config.vendorId));
				LogInfo("Renderer GPU device: %u.", config.deviceId);
			}
			bgfxInfo.deviceId = config.deviceId;
			bgfxInfo.vendorId = config.vendorId;
			bgfx::shutdown();
		}
		else
			break;
	}

	bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, config.clear_color << 8 | 0xff, 1.0f, 0);

	world.add<Renderer>();

	LogInfo("BGFX Renderer has been initialised.");

	return true;
}

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

inline static bool normalize_config(RendererConfig& config, SDL_Window* window, const flecs::world& world) noexcept
{
	bool modified = false;
	if (config.width == 0 || config.height == 0)
	{
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		config.width = (unsigned)width;
		config.height = (unsigned)height;
		modified = true;
	}
	return modified;
}

inline static unsigned parse_flags(const RendererConfig& config) noexcept
{
	unsigned flags = BGFX_RESET_HIDPI;
	if (config.fullscreen)	flags |= BGFX_RESET_FULLSCREEN;
	if (config.vsync)		flags |= BGFX_RESET_VSYNC;
	switch (config.msaa)
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

static void frame_begin_st(const RendererConfig& config)
{
	LogTrace("Render frame begin start...");

	bgfx::setViewRect(0, 0, 0, (uint16_t)config.width, (uint16_t)config.height);
	bgfx::touch(0);

	LogTrace("Render frame begin finish...");
}

static void frame_end_st(flecs::iter&, size_t)
{
	LogTrace("Render frame end start...");

	bgfx::frame();

	LogTrace("Render frame end finish...");
}

static void frame_begin_mt(flecs::iter& it, size_t, const Renderer& renderer, const RendererConfig& config)
{
	LogTrace("Render frame begin start...");

	bgfx::setViewRect(0, 0, 0, (uint16_t)config.width, (uint16_t)config.height);
	bgfx::touch(0);

	flecs::world w = it.world();
	for (unsigned i = 0; i < w.get_threads(); ++i)
	{
		renderer.threads[i].queue = bgfx::begin(i != 0);
		Assert(renderer.threads[i].queue, "Renderer threads' queue is NULL.");
	}

	LogTrace("Render frame begin finish...");
}

static void frame_end_mt(flecs::iter& it, size_t, const Renderer& renderer, const RendererConfig& config)
{
	LogTrace("Render frame end start...");

	flecs::world w = it.world();
	for (unsigned i = 0; i < w.get_threads(); ++i)
	{
		Assert(renderer.threads[i].queue, "Renderer threads' queue is NULL.");
		bgfx::end(renderer.threads[i].queue);
	}

	bgfx::frame();

	LogTrace("Render frame end finish...");
}

static void update_config(flecs::iter& it, size_t, const RendererConfig& config)
{
	flecs::world w = it.world();

	w.set<Aspect>({(float)config.width / (float)config.height});

	if (config.multi_threaded)
		w.add<MultiThreaded>(); // Switch to multi-threaded
	else
		w.remove<MultiThreaded>(); // Switch to single-threaded

	Renderer* renderer = w.get_mut<Renderer>();
	if (renderer)
		renderer->threads.resize(w.get_threads());
}

static void destroy_renderer(Renderer&) { bgfx::shutdown(); }

RendererSystems::RendererSystems(flecs::world& world)
{
	world.module<RendererSystems>("A3D::Systems::Renderer");

	updateConfig_ = world.observer<const RendererConfig>("UpdateConfig")
		.arg(1).singleton()
		.event(flecs::OnSet)
		.each(update_config);

	unSetRenderer_ = world.observer<Renderer>("DestroyRenderer")
		.arg(1).singleton()
		.event(flecs::UnSet)
		.each(destroy_renderer);

	frameBeginSingleThreaded_ = world.system<const RendererConfig>("FrameBeginSingleThreaded")
								.term<const Renderer>().singleton()
								.term<const MultiThreaded>().not_().singleton()
								.kind(flecs::PostUpdate)
								.each(frame_begin_st);

	frameEndSingleThreaded_ = world.system<>("FrameEndSingleThreaded")
							  .term<const Renderer>().singleton()
							  .term<const RendererConfig>().singleton()
							  .term<const MultiThreaded>().not_().singleton()
							  .kind(flecs::OnStore)
							  .each(frame_end_st);

	frameBeginMultiThreaded_ = world.system<const Renderer, const RendererConfig>("FramBeginMultiThreaded")
							   .arg(1).singleton()
							   .arg(2).singleton()
							   .term<const MultiThreaded>().singleton()
							   .kind(flecs::PostUpdate)
							   .each(frame_begin_mt);

	frameEndMultiThreaded_ = world.system<const Renderer, const RendererConfig>("FramEndMultiThreaded")
							 .arg(1).singleton()
							 .arg(2).singleton()
							 .term<const MultiThreaded>().singleton()
							 .kind(flecs::OnStore)
							 .each(frame_end_mt);
}
} // namespace A3D
