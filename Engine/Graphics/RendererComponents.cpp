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

#include "IO/Log.h"
#include "RendererComponents.h"

#ifndef BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#define BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif // BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

using namespace A3D::Components::Renderer;

namespace A3D
{
RendererComponents::RendererComponents(flecs::world& world)
{
	world.module<RendererComponents>("A3D::Components::Renderer");

	msaaLevel_ = world.component<MsaaLevel>()
		.constant("NONE", (int)MsaaLevel::NONE)
		.constant("X2", (int)MsaaLevel::X2)
		.constant("X4", (int)MsaaLevel::X4)
		.constant("X8", (int)MsaaLevel::X8)
		.constant("X16", (int)MsaaLevel::X16);

	rendererType_ = world.component<RendererType>()
		.constant("Auto", (int)RendererType::Auto)
		.constant("OpenGL", (int)RendererType::OpenGL)
		.constant("Vulkan", (int)RendererType::Vulkan)
#if defined(__WIN32__)
		.constant("Direct3D9", (int)RendererType::Direct3D9)
		.constant("Direct3D11", (int)RendererType::Direct3D11)
		.constant("Direct3D12", (int)RendererType::Direct3D12)
#elif defined(OSX)
		.constant("Metal", (int)RendererType::Metal)
#endif // defined
		.constant("None", (int)RendererType::None);

	aspect_ = world.component<Aspect>().member<float>("ratio");

	rendererConfig_ = world.component<RendererConfig>()
		.member<uint32_t>("clear_color")
		.member<uint16_t>("width")
		.member<uint16_t>("height")
		.member<uint16_t>("deviceId")
		.member<uint16_t>("vendorId")
		.member<RendererType>("type")
		.member<MsaaLevel>("msaa")
		.member<bool>("multi_threaded")
		.member<bool>("fullscreen")
		.member<bool>("vsync");

	renderer_ = world.component<Renderer>();

	multiThreaded_ = world.component<MultiThreaded>().add(flecs::Tag);
}

RendererAllocator::~RendererAllocator()
{
}

void* RendererAllocator::realloc(void* _ptr,
								size_t _size,
								size_t _align,
								const char* _file,
								uint32_t _line)
{
	if (0 == _size)
	{
		if (NULL != _ptr)
		{
			if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
			{
				::free(_ptr);
				return NULL;
			}

#if BX_COMPILER_MSVC
			BX_UNUSED(_file, _line);
			_aligned_free(_ptr);
#else
			alignedFree(this, _ptr, _align, _file, _line);
#endif // BX_
		}

		return NULL;
	}
	else if (NULL == _ptr)
	{
		if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
		{
			return ::malloc(_size);
		}

#if BX_COMPILER_MSVC
		BX_UNUSED(_file, _line);
		return _aligned_malloc(_size, _align);
#else
		return alignedAlloc(this, _size, _align, _file, _line);
#endif // BX_
	}

	if (BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT >= _align)
	{
		return ::realloc(_ptr, _size);
	}

#if BX_COMPILER_MSVC
	BX_UNUSED(_file, _line);
	return _aligned_realloc(_ptr, _size, _align);
#else
	return alignedRealloc(this, _ptr, _size, _align, _file, _line);
#endif // BX_
}

RendererCallback::~RendererCallback()
{
}

void RendererCallback::fatal(const char* _filePath,
							 uint16_t _line,
							 bgfx::Fatal::Enum _code,
							 const char* _str)
{
	char message[512];
	sprintf(message, "Renderer error: %s", _str);
	message[strlen(message)] = '.';
	LogError(message);
	exit((int)_code);
}

void RendererCallback::traceVargs(const char* _filePath,
								  uint16_t _line,
								  const char* _format,
								  va_list _argList)
{
	char message[512];
	vsprintf(message, _format, _argList);
	LogTrace(message);
}

void RendererCallback::profilerBegin(const char* _name,
									 uint32_t _abgr,
									 const char* _filePath,
									 uint16_t _line)
{
}

void RendererCallback::profilerBeginLiteral(const char* _name,
											uint32_t _abgr,
											const char* _filePath,
											uint16_t _line)
{
}

void RendererCallback::profilerEnd()
{
}

uint32_t RendererCallback::cacheReadSize(uint64_t _id)
{
	return 0;
}

bool RendererCallback::cacheRead(uint64_t _id, void* _data, uint32_t _size)
{
	return false;
}

void RendererCallback::cacheWrite(uint64_t _id, const void* _data, uint32_t _size)
{

}

void RendererCallback::screenShot(const char* _filePath,
								  uint32_t _width,
								  uint32_t _height,
								  uint32_t _pitch,
								  const void* _data,
								  uint32_t _size,
								  bool _yflip)
{

}

void RendererCallback::captureBegin(uint32_t _width,
									uint32_t _height,
									uint32_t _pitch,
									bgfx::TextureFormat::Enum _format,
									bool _yflip)
{
}

void RendererCallback::captureEnd()
{
}

void RendererCallback::captureFrame(const void* _data, uint32_t _size)
{
}
} // namespace A3D
