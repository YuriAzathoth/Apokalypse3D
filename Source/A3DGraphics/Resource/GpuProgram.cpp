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
/*#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/readerwriter.h>*/
#include <stdio.h>
#include "GpuProgram.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

namespace A3D
{
const char* GetShaderPrefix()
{
	switch (bgfx::getRendererType())
	{
	case bgfx::RendererType::Direct3D9:
		return "dx9";
	case bgfx::RendererType::Direct3D11:
	case bgfx::RendererType::Direct3D12:
		return "dx11";
	case bgfx::RendererType::Metal:
		return "msl";
	case bgfx::RendererType::OpenGLES:
		return "essl";
	case bgfx::RendererType::OpenGL:
		return "glsl";
	case bgfx::RendererType::Vulkan:
		return "spirv";
	default:
		LogFatal("Invalid BGFX renderer mode.");
		return nullptr;
	}
}

bool LoadShaderFromMemory(Shader& shader, const char* shader_text, uint32_t shader_size)
{
	const bgfx::Memory* mem = bgfx::makeRef(shader_text, shader_size);
	shader.handle = bgfx::createShader(mem);
	if (bgfx::isValid(shader.handle))
	{
		LogInfo("Custom shader has been loaded.");
		return true;
	}
	else
	{
		LogFatal("Custom shader compilation error.");
		return false;
	}
}

bool LoadShaderFromFile(Shader& shader, const char* filename)
{
	LogDebug("Loading shader \"%s\"...", filename);

	File file;
	if (!IsFileExists(filename))
	{
		LogFatal("Could not load shader: file \"%s\" not found.", filename);
		return false;
	}

	if (!OpenFileRead(file, filename))
		return false;

	if (file.size == 0)
	{
		LogFatal("Could not load shader: file \"%s\" is empty.", filename);
		CloseFile(file);
		return false;
	}

	const bgfx::Memory* mem = bgfx::alloc(file.size + 1);
	ReadFileData(file, mem->data);
	CloseFile(file);

	shader.handle = bgfx::createShader(mem);
	if (bgfx::isValid(shader.handle))
	{
		LogInfo("Shader \"%s\" has been loaded.", filename);
		return true;
	}
	else
	{
		LogFatal("Shader \"%s\" compilation error.", filename);
		return false;
	}
}

void DestroyShader(Shader& shader) { bgfx::destroy(shader.handle); }

bool LinkGpuProgram(GpuProgram& program, const Shader& vertex, const Shader& fragment)
{
	LogDebug("Begin linking GPU program...");
	Assert(bgfx::isValid(vertex.handle), "Could not link program to invalid vertex shader.");
	Assert(bgfx::isValid(fragment.handle), "Could not link program to invalid fragment shader.");
	Assert(vertex.handle.idx != fragment.handle.idx, "Could not link program to same vertex and fragment shader.");

	program.handle = bgfx::createProgram(vertex.handle, fragment.handle);
	if (bgfx::isValid(program.handle))
	{
		LogInfo("GPU program has been linked.");
		return true;
	}
	else
	{
		LogFatal("Failed to link GPU program.");
		return false;
	}
}

void DestroyGpuProgram(GpuProgram& program) { bgfx::destroy(program.handle); }
} // namespace A3D
