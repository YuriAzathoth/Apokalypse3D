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

#include <bgfx/bgfx.h>
#include <unordered_map>
#include "Container/dense_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "Core/EngineLog.h"
#include "IO/File.h"
#include "Shader.h"

#define BUFFER_SIZE 256
#define SHADER_TIMER 20

namespace A3D
{
using RefsCount = uint16_t;
using ResourceIndex = uint16_t;
using ShaderHandleType = decltype(bgfx::ShaderHandle::idx);
using TimerType = uint8_t;

struct ShaderCache
{
	std::unordered_map<string_hash, ShaderHandleType> by_name;
	std::unordered_map<ShaderHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, ShaderHandleType> shaders;
	dense_map<ResourceIndex, TimerType> timers;
	dense_map<ResourceIndex, string> filenames;
};

static ShaderCache s_cache;

extern const char* s_shader_suffix;

static bool LoadShaderFile(Shader& shader, const char* filename)
{
	char real_filename[BUFFER_SIZE];
	sprintf(real_filename, "%s/%s", s_shader_suffix, filename);

	File file;
	if (!OpenFileRead(file, real_filename))
	{
		LogFatal("Could not load shader \"%s\": file does not exist.", real_filename);
		return false;
	}

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
		LogInfo("Shader \"%s\" has been loaded.", real_filename);
		return true;
	}
	else
	{
		LogFatal("Could not load shader \"%s\": compilation error.", real_filename);
		return false;
	}
}

bool GetShader(Shader& shader, const char* filename)
{
	const string_hash filename_hash = filename;
	const auto it = s_cache.by_name.find(filename_hash);
	if (it != s_cache.by_name.end())
	{
		s_cache.timers[s_cache.indices[it->second]] = SHADER_TIMER;
		shader.handle.idx = it->second;
		return true;
	}
	else if (LoadShaderFile(shader, filename))
	{
		s_cache.by_name.emplace(filename, shader.handle.idx);
		s_cache.indices.emplace(shader.handle.idx, s_cache.shaders.size());

		s_cache.shaders.insert(shader.handle.idx);
		s_cache.timers.insert(SHADER_TIMER);
		s_cache.filenames.insert(filename);

		return true;
	}
	else
		return false;
}

void UpdateShaderCache()
{
	ResourceIndex index;
	ResourceIndex rebound;
	bgfx::ShaderHandle handle;
	auto it = s_cache.timers.begin();
	while (it != s_cache.timers.end())
	{
		if (--*it > 0)
			++it;
		else
		{
			index = (ResourceIndex)(it - s_cache.timers.begin());

			handle.idx = s_cache.shaders[index];
			bgfx::destroy(handle);

			s_cache.by_name.erase(s_cache.filenames[index]);
			s_cache.indices.erase(handle.idx);

			rebound = s_cache.shaders.erase(index);
			s_cache.timers.erase(index);
			s_cache.filenames.erase(index);

			if (rebound != s_cache.shaders.INVALID_KEY)
				s_cache.indices[s_cache.shaders[index]] = index;
		}
	}
}
} // namespace A3D
