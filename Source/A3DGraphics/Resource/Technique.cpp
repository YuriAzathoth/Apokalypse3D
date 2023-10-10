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

#include <string.h>
#include <unordered_map>
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"
#include "Shader.h"
#include "Technique.h"

#define RAPIDXML_NO_EXCEPTIONS
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#define BUFFER_SIZE 256

namespace A3D
{
using RefsCount = uint16_t;
using ResourceIndex = uint16_t;
using UniformHandleType = decltype(bgfx::UniformHandle::idx);
using UniformType = bgfx::UniformType::Enum;

struct TechniqueCache
{
	std::unordered_map<string_hash, TechniqueHandle> by_name;
	sparse_map<TechniqueHandle, ResourceIndex> indices;

	dense_map<ResourceIndex, bgfx::ProgramHandle> programs;
	dense_map<ResourceIndex, RefsCount> refs;
	dense_map<ResourceIndex, string> filenames;
	dense_map<ResourceIndex, TechniqueHandle> handles;
};

struct UniformStorage
{
	std::unordered_map<string_hash, Uniform> by_name;

	// TODO: Change to sparse map.
	std::unordered_map<UniformHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, string> names;
	dense_map<ResourceIndex, UniformType> types;

	// TODO: Implement techniques containing uniforms container.
//	std::unordered_multimap<TechniqueHandle, ResourceIndex> techniques;
};

static TechniqueCache s_cache;
static UniformStorage s_uniforms;

inline static UniformType GetUniformType(const char* type)
{
	// Fastest string parsing.
	if (type[0] == 'v' && type[1] == 'e' && type[2] == 'c' && type[3] == '4' && type[4] == '\0')
		return UniformType::Vec4;
	else if (type[0] == 'm' && type[1] == 'a' && type[2] == 't' && type[4] == '\0')
	{
		if (type[3] == '3')
			return UniformType::Mat3;
		if (type[3] == '4')
			return UniformType::Mat4;
	}
	return UniformType::End;
}

static bool LoadTechniqueFile(Technique technique, const char* filename)
{
	File file;
	if (!OpenFileRead(file, filename))
	{
		LogFatal("Could not load render technique \"%s\": file does not exist.", filename);
		return false;
	}

	char* text = (char*)malloc(file.size + 1);
	if (text == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": out of memory.", filename);
		CloseFile(file);
		return false;
	}

	if (!ReadFileData(file, text) || file.size == 0)
	{
		LogFatal("Could not load render technique \"%s\": file reading error.", filename);
		free(text);
		CloseFile(file);
		return false;
	}
	text[file.size] = '\0';

	rapidxml::xml_document<> doc;
	doc.parse<0>(text);

	CloseFile(file);

	rapidxml::xml_node<>* root = doc.first_node("technique");
	if (root == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": invalid file format.", filename);
		free(text);
		return false;
	}

	char vertex_filename[BUFFER_SIZE];
	char fragment_filename[BUFFER_SIZE];

	rapidxml::xml_attribute<>* attrib = root->first_attribute("vs");
	if (attrib == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": vertex shader is not specialized.", filename);
		free(text);
		return false;
	}
	strcpy(vertex_filename, attrib->value());

	attrib = root->first_attribute("fs");
	if (attrib == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": fragment shader is not specialized.", filename);
		free(text);
		return false;
	}
	strcpy(fragment_filename, attrib->value());

	const ResourceIndex index = s_cache.handles.size();

	rapidxml::xml_node<>* node;
	const char* name_str;
	const char* type_str;
	Uniform uniform;
	UniformType type;
	ResourceIndex uniform_index;
	for (node = root->first_node("parameter"); node != nullptr; node = node->next_sibling("parameter"))
	{
		attrib = node->first_attribute("name");
		if (attrib == nullptr)
		{
			LogFatal("Could not load render technique \"%s\": parameter does not have a name.", filename);
			free(text);
			return false;
		}
		name_str = attrib->value();

		attrib = node->first_attribute("type");
		if (attrib == nullptr)
		{
			LogFatal("Could not load render technique \"%s\": parameter does not have a type.", filename);
			free(text);
			return false;
		}
		type_str = attrib->value();

		type = GetUniformType(type_str);
		if (type == bgfx::UniformType::End)
		{
			LogFatal("Could not load render technique \"%s\": unsupported technique parameter.", filename);
			free(text);
			return false;
		}

		auto it = s_uniforms.by_name.find(name_str);
		if (it != s_uniforms.by_name.end())
		{
			const ResourceIndex index = s_uniforms.indices[it->second.handle.idx];

			if (type != s_uniforms.types[index]);
			{
				LogFatal("Could not load render technique \"%s\": using uniform with name type but with different type.", filename);
				free(text);
				return false;
			}

//			s_uniforms.techniques.emplace(technique_index, it->second);
		}
		else
		{
			uniform.handle = bgfx::createUniform(name_str, type, 1);
			if (!bgfx::isValid(uniform.handle))
			{
				LogFatal("Could not load render technique \"%s\": uniform creation error.", filename);
				free(text);
				return false;
			}

			uniform_index = s_uniforms.names.size();

			s_uniforms.by_name.emplace(name_str, uniform);
			s_uniforms.indices.emplace(uniform.handle.idx, uniform_index);
			s_uniforms.names.emplace(name_str);
			s_uniforms.types.emplace(type);
		}
	}

	free(text);

	Shader vertex;
	Shader fragment;

	if (!GetShader(vertex, vertex_filename))
	{
		LogFatal("Could not load render technique \"%s\": vertex shader loading error.", filename);
		free(text);
		return false;
	}
	if (!GetShader(fragment, fragment_filename))
	{
		LogFatal("Could not load render technique \"%s\": fragment shader loading error.", filename);
		free(text);
		return false;
	}

	const bgfx::ProgramHandle program = bgfx::createProgram(vertex.handle, fragment.handle);
	if (!bgfx::isValid(program))
	{
		LogFatal("Could not load render technique \"%s\": GPU program linking error.", filename);
		return false;
	}

	technique.handle = s_cache.indices.insert(index);

	s_cache.by_name.emplace(filename, technique.handle);
	s_cache.programs.insert(program);
	s_cache.refs.insert(1);
	s_cache.filenames.emplace(filename);
	s_cache.handles.insert(technique.handle);

	LogInfo("Render technique \"%s\" loaded.", filename);
	return true;
}

bool GetTechnique(Technique& technique, const char* filename)
{
	const string_hash filename_hash = filename;
	const auto it = s_cache.by_name.find(filename_hash);
	if (it != s_cache.by_name.end())
	{
		technique.handle = it->second;
		++s_cache.refs[it->second];
		return true;
	}
	else
		return LoadTechniqueFile(technique, filename);
}

void ReleaseTechnique(Technique technique)
{
	const ResourceIndex index = s_cache.indices[technique.handle];
	if (--s_cache.refs[index] == 0)
	{
		bgfx::destroy(s_cache.programs[index]);

		s_cache.by_name.erase(s_cache.filenames[index]);
		s_cache.indices.erase(technique.handle);

		const ResourceIndex rebound = s_cache.programs.erase(index);
		s_cache.refs.erase(index);
		s_cache.filenames.erase(index);
		s_cache.handles.erase(index);

		if (rebound != s_cache.handles.INVALID_KEY)
			s_cache.indices[s_cache.handles[index]] = index;
	}
}

bgfx::ProgramHandle GetTechniqueProgram(Technique technique)
{
	return s_cache.programs[s_cache.indices[technique.handle]];
}




Uniform GetUniform(const char* name)
{
	const auto it = s_uniforms.by_name.find(name);
	if (it != s_uniforms.by_name.end())
		return it->second;
	else
	{
		LogFatal("Could not get shader uniform \"%s\": not registered yet.", name);
		return { BGFX_INVALID_HANDLE };
	}
}

uint16_t GetUniformSize(Uniform uniform)
{
	switch (s_uniforms.types[s_uniforms.indices.find(uniform.handle.idx)->second])
	{
	case bgfx::UniformType::Mat3:
		return sizeof(float) * 3 * 3;
	case bgfx::UniformType::Mat4:
		return sizeof(float) * 4 * 4;
	case bgfx::UniformType::Vec4:
		return sizeof(float) * 4;
	default:
		return 0;
	}
}

bgfx::UniformType::Enum GetUniformType(Uniform uniform)
{
	return s_uniforms.types[s_uniforms.indices.find(uniform.handle.idx)->second];
}
} // namespace A3D
