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

#ifndef GRAPHICS_H
#define GRAPHICS_H

#define EASTL_DLL 1

#include <EASTL/string.h>
#include <EASTL/unordered_map.h>
#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <flecs.h>
#include <glm/mat4x4.hpp>
#include "Apokalypse3DAPI.h"

struct SDL_Window;
typedef void* SDL_GL_Context;

class APOKALYPSE3DAPI_EXPORT Graphics
{
public:
	enum class RenderType : unsigned
	{
		AUTO,
		OPENGL,
		VULKAN,
		METAL,
		DIRECTX9,
		DIRECTX11,
		DIRECTX12
	};

	struct InitInfo
	{
		const char* title;
		int width;
		int height;
		bool vsync;
	};

	Graphics(const InitInfo initInfo, bool& initialized);
	~Graphics();

	void BeginFrame() noexcept;
	void EndFrame() noexcept;

	void DrawModel(bgfx::ProgramHandle program,
				   bgfx::VertexBufferHandle vbo,
				   bgfx::IndexBufferHandle ebo,
				   const glm::mat4& model) const noexcept;
	void DrawScene(flecs::entity node) const noexcept;

	void SetTexture(bgfx::TextureHandle texture, unsigned short slot) const noexcept;

	bgfx::ShaderHandle LoadShader(const char* filename) noexcept;
	bgfx::ProgramHandle CreateProgram(bgfx::ShaderHandle vertexShader, bgfx::ShaderHandle fragmentShader) noexcept;

	bgfx::VertexBufferHandle CreateVertexBuffer(const void* data, unsigned size) noexcept;
	bgfx::IndexBufferHandle CreateIndexBuffer(const unsigned short* data, unsigned size) noexcept;

	bgfx::TextureHandle LoadTexture(const char* filename) noexcept;

	bgfx::UniformHandle GetUniformHandle(const char* name) const noexcept
	{
		const auto it = uniforms_.find(name);
		return it != uniforms_.end() ? it->second : bgfx::UniformHandle BGFX_INVALID_HANDLE;
	}

	static const char* GetShadersPath() noexcept;

private:
	bgfx::VertexLayout solidLayout_;
	bx::DefaultAllocator allocator_;
	eastl::unordered_map<eastl::string, bgfx::UniformHandle> uniforms_;
	SDL_Window* window_;
};

#endif // GRAPHICS_H
