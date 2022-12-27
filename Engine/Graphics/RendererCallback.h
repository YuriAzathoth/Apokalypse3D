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

#ifndef RENDERERCALLBACK_H
#define RENDERERCALLBACK_H

#include <bgfx/bgfx.h>

namespace A3D
{
struct RendererCallback : public bgfx::CallbackI
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
} // namespace A3D

#endif // RENDERERCALLBACK_H
