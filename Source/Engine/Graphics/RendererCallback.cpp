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

#include <stdio.h>
#include "Core/ILog.h"
#include "RendererCallback.h"

#ifndef BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT
#define BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT 8
#endif // BX_CONFIG_ALLOCATOR_NATURAL_ALIGNMENT

namespace A3D
{
void RendererCallback::fatal(const char* _filePath,
							 uint16_t _line,
							 bgfx::Fatal::Enum _code,
							 const char* _str)
{
	log_->Fatal("Renderer error: %s", _str);
}

void RendererCallback::traceVargs(const char* _filePath,
								  uint16_t _line,
								  const char* _format,
								  va_list _argList)
{
	char message[512];
	vsprintf(message, _format, _argList);
	log_->Trace(message);
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
