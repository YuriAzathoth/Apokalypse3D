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

#ifndef RENDERERCOMPONENTS_H
#define RENDERERCOMPONENTS_H

#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <flecs.h>
#include <vector>
#include "Apokalypse3DAPI.h"

namespace bgfx { struct Encoder; }

namespace A3D
{
namespace Components
{
namespace Renderer
{
enum class MsaaLevel : unsigned
{
	NONE,
	X2,
	X4,
	X8,
	X16
};

enum class RendererType : unsigned
{
	Auto,
	OpenGL,
	Vulkan,
#if defined(__WIN32__)
	Direct3D9,
	Direct3D11,
	Direct3D12,
#elif defined(OSX)
	Metal,
#endif // defined
	None
};

struct APOKALYPSE3DAPI_EXPORT RendererAllocator : public bx::AllocatorI
{
	~RendererAllocator();
	void* realloc(void* _ptr,
				  size_t _size,
				  size_t _align,
				  const char* _file,
				  uint32_t _line) override;
};

struct APOKALYPSE3DAPI_EXPORT RendererCallback : public bgfx::CallbackI
{
	~RendererCallback();
	void fatal(const char* _filePath,
			   uint16_t _line,
			   bgfx::Fatal::Enum _code,
			   const char* _str) override;
	void traceVargs(const char* _filePath,
					uint16_t _line,
					const char* _format,
					va_list _argList) override;
	void profilerBegin(const char* _name,
					   uint32_t _abgr,
					   const char* _filePath,
					   uint16_t _line) override;
	void profilerBeginLiteral(const char* _name,
							  uint32_t _abgr,
							  const char* _filePath,
							  uint16_t _line) override;
	void profilerEnd() override;
	uint32_t cacheReadSize(uint64_t _id) override;
	bool cacheRead(uint64_t _id, void* _data, uint32_t _size) override;
	void cacheWrite(uint64_t _id, const void* _data, uint32_t _size) override;
	void screenShot(const char* _filePath,
					uint32_t _width,
					uint32_t _height,
					uint32_t _pitch,
					const void* _data,
					uint32_t _size,
					bool _yflip) override;
	void captureBegin(uint32_t _width,
					  uint32_t _height,
					  uint32_t _pitch,
					  bgfx::TextureFormat::Enum _format,
					  bool _yflip) override;
	void captureEnd() override;
	void captureFrame(const void* _data, uint32_t _size) override;
};

struct Thread
{
	mutable bgfx::Encoder* queue;
};

struct Aspect
{
	float ratio;
};

struct RendererConfig
{
	uint32_t clear_color;
	uint16_t width;
	uint16_t height;
	uint16_t deviceId;
	uint16_t vendorId;
	enum RendererType type;
	enum MsaaLevel msaa;
	bool multi_threaded;
	bool fullscreen;
	bool vsync;
};

struct Renderer
{
	std::vector<Thread> threads;
	bx::DefaultAllocator alloc;
};

struct MultiThreaded {};
} // namespace Renderer
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT RendererComponents
{
	RendererComponents(flecs::world& world);

	flecs::entity msaaLevel_;
	flecs::entity rendererType_;

	flecs::entity aspect_;
	flecs::entity rendererConfig_;

	flecs::entity renderer_;

	flecs::entity multiThreaded_;
};
} // namespace A3D

#endif // RENDERERCOMPONENTS_H
