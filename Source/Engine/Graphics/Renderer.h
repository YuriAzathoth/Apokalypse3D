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

#ifndef GRAPHICS_RENDERER_H
#define GRAPHICS_RENDERER_H

#include <stdint.h>
#include "Common/Config.h"
#include "Container/vector.h"
#include "EngineAPI.h"
#include "GraphicsConfig.h"

namespace bx { struct AllocatorI; }

namespace bgfx
{
struct CallbackI;
struct Encoder;
}

namespace A3D
{
class ILog;

class ENGINEAPI_EXPORT Renderer
{
public:
	Renderer(ILog* log, bx::AllocatorI* alloc, bgfx::CallbackI* callback);
	~Renderer();

	bool Initialize(GraphicsConfig& config, WindowPlatformData& wpd);
	void Shutdown();

	void BeginFrame();
	void EndFrame();

	const char* GetShaderPrefix() const { return shader_prefix_; }

private:
	bool SelectBestGpu(GraphicsConfig& config);

	vector<uint8_t, bgfx::Encoder*> thread_encoders_;
	ScreenResolution resolution_;
	const char* shader_prefix_;
	ILog* log_;
	bx::AllocatorI* alloc_;
	bgfx::CallbackI* callback_;

public:
	static bool IsInitialized() { return s_initialized; }

private:
	static bool s_initialized;
};
} // namespace A3D

#endif // GRAPHICS_RENDERER_H
