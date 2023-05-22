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
#include <stdio.h>
#include <string.h>
#include <unordered_map>
#include "Common/GpuProgram.h"
#include "Container/dense_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "GpuProgram.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"

#define BUFFER_SIZE 256
#define SHADER_TIMER 20

namespace A3D
{
struct Shader
{
	bgfx::ShaderHandle handle;
};

struct GpuProgramParts
{
	string_hash vertex_shader;
	string_hash fragment_shader;
};

struct GpuProgramPartsEqualTo
{
	bool operator()(const GpuProgramParts& left, const GpuProgramParts& right) const
	{
		return left.vertex_shader == right.fragment_shader;
	}
};

struct GpuProgramPartsHash
{
	size_t operator()(const GpuProgramParts& key) const
	{
		return key.vertex_shader.hash() ^ key.fragment_shader.hash();
	}
};

using GpuProgramHandleType = decltype(bgfx::ProgramHandle::idx);
using RefsCount = uint16_t;
using ResourceIndex = uint16_t;
using ShaderHandleType = decltype(bgfx::ShaderHandle::idx);
using TimerType = uint8_t;

struct GpuProgramCache
{
	std::unordered_map<GpuProgramParts, GpuProgramHandleType, GpuProgramPartsHash, GpuProgramPartsEqualTo> by_name;
	std::unordered_map<GpuProgramHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, GpuProgramHandleType> programs;
	dense_map<ResourceIndex, RefsCount> refs;
	dense_map<ResourceIndex, GpuProgramParts> parts;
};

struct ShaderCache
{
	std::unordered_map<string_hash, ShaderHandleType> by_name;
	std::unordered_map<ShaderHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, ShaderHandleType> shaders;
	dense_map<ResourceIndex, TimerType> timers;
	dense_map<ResourceIndex, string> filenames;
};

static GpuProgramCache s_programs_cache;
static ShaderCache s_shaders_cache;

static bool GetShader(Shader& shader, const char* filename);

bool GetGpuProgram(GpuProgram& program, const char* vertex_filename, const char* fragment_filename)
{
	GpuProgramParts key;
	key.vertex_shader = vertex_filename;
	key.fragment_shader = fragment_filename;

	const auto it = s_programs_cache.by_name.find(key);
	if (it != s_programs_cache.by_name.end())
	{
		++s_programs_cache.refs[s_programs_cache.indices[it->second]];
		program.handle.idx = it->second;
		return true;
	}
	else
	{
		Shader vertex;
		Shader fragment;
		if (!GetShader(vertex, vertex_filename) || !GetShader(fragment, fragment_filename))
			return false;

		program.handle = bgfx::createProgram(vertex.handle, fragment.handle);
		if (bgfx::isValid(program.handle))
		{
			s_programs_cache.by_name.emplace(key, program.handle.idx);
			s_programs_cache.indices.emplace(program.handle.idx, s_programs_cache.programs.size());

			s_programs_cache.programs.insert(program.handle.idx);
			s_programs_cache.refs.insert(1);
			s_programs_cache.parts.insert(key);

			LogInfo("GPU program with shaders \"%s\" and \"%s\" has been loaded.", vertex_filename, fragment_filename);

			return true;
		}
		else
		{
			LogFatal("Could not load GPU program with shaders \"%s\" and \"%s\" : linking error.", vertex_filename, fragment_filename);
			return false;
		}
	}
}

void ReleaseGpuProgram(GpuProgram& program)
{
	const ResourceIndex index = s_programs_cache.indices[program.handle.idx];
	if (--s_programs_cache.refs[index] == 0)
	{
		bgfx::destroy(program.handle);

		s_programs_cache.by_name.erase(s_programs_cache.parts[index]);
		s_programs_cache.indices.erase(program.handle.idx);

		const ResourceIndex rebound = s_programs_cache.programs.erase(index);
		s_programs_cache.parts.erase(index);
		s_programs_cache.refs.erase(index);

		if (rebound != s_programs_cache.programs.INVALID_KEY)
			s_programs_cache.indices[s_programs_cache.programs[index]] = index;
	}
}

void UpdateGpuProgramCache()
{
	ResourceIndex index;
	ResourceIndex rebound;
	bgfx::ShaderHandle handle;
	auto it = s_shaders_cache.timers.begin();
	while (it != s_shaders_cache.timers.end())
	{
		if (--*it > 0)
			++it;
		else
		{
			index = (ResourceIndex)(it - s_shaders_cache.timers.begin());

			handle.idx = s_shaders_cache.shaders[index];
			bgfx::destroy(handle);

			s_shaders_cache.by_name.erase(s_shaders_cache.filenames[index]);
			s_shaders_cache.indices.erase(handle.idx);

			rebound = s_shaders_cache.shaders.erase(index);
			s_shaders_cache.timers.erase(index);
			s_shaders_cache.filenames.erase(index);

			if (rebound != s_programs_cache.programs.INVALID_KEY)
				s_shaders_cache.indices[s_shaders_cache.shaders[index]] = index;
		}
	}
}

inline static const char* GetShaderExt()
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

bool GetShader(Shader& shader, const char* filename)
{
	const string_hash filename_hash = filename;
	const auto it = s_shaders_cache.by_name.find(filename_hash);
	if (it != s_shaders_cache.by_name.end())
	{
		s_shaders_cache.timers[s_shaders_cache.indices[it->second]] = SHADER_TIMER;
		shader.handle.idx = it->second;
		return true;
	}
	else
	{
		char real_filename[BUFFER_SIZE];
		sprintf(real_filename, "%s.%s", filename, GetShaderExt());

		File file;
		if (!OpenFileRead(file, real_filename))
			return false;

		const bgfx::Memory* mem = bgfx::alloc(file.size);
		if (mem == nullptr)
		{
			LogFatal("Could not load shader \"%s\": out of memory.", real_filename);
			CloseFile(file);
			return false;
		}

		if (!ReadFileData(file, mem->data))
		{
			LogFatal("Could not load shader \"%s\": file reading error.", real_filename);
			CloseFile(file);
			return false;
		}

		shader.handle = bgfx::createShader(mem);

		CloseFile(file);

		if (bgfx::isValid(shader.handle))
		{
			s_shaders_cache.by_name.emplace(filename, shader.handle.idx);
			s_shaders_cache.indices.emplace(shader.handle.idx, s_shaders_cache.shaders.size());

			s_shaders_cache.shaders.insert(shader.handle.idx);
			s_shaders_cache.timers.insert(SHADER_TIMER);
			s_shaders_cache.filenames.insert(filename);

			LogInfo("Shader \"%s\" has been loaded.", real_filename);

			return true;
		}
		else
		{
			LogFatal("Could not load shader \"%s\": compilation error.", real_filename);
			return false;
		}
	}
}
} // namespace A3D
