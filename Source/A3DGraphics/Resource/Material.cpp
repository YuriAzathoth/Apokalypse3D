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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unordered_map>
#include "Common/Technique.h"
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/string.h"
#include "Container/string_hash.h"
#include "IO/FileSystem.h"
#include "IO/Log.h"
#include "Material.h"
#include "Technique.h"

#define RAPIDXML_NO_EXCEPTIONS
#include <rapidxml/rapidxml.hpp>
#include <rapidxml/rapidxml_utils.hpp>

#define BUFFER_SIZE 256

namespace A3D
{
using MaterialHandleType = uint16_t;
using RefsCount = uint16_t;
using ResourceIndex = uint16_t;

// TODO: Make appropriate allocation/deallocation.
struct UniformPair
{
	Uniform uniform;
	char* data;
};

struct MaterialCache
{
	std::unordered_map<string_hash, MaterialHandleType> by_name;
	sparse_map<MaterialHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, Technique> techniques;
	dense_map<ResourceIndex, RefsCount> refs;
	dense_map<ResourceIndex, string> filenames;
	dense_map<ResourceIndex, MaterialHandleType> handles;

	std::unordered_multimap<MaterialHandleType, UniformPair> uniforms;
};

static MaterialCache s_cache;

static void ParseUniformParameter(void* data, const char* str, bgfx::UniformType::Enum type)
{
	switch (type)
	{
	case bgfx::UniformType::Vec4:
		{
			float* ptr = (float*)data;
			sscanf(str, "%f %f %f %f", ptr, ptr + 1, ptr + 2, ptr + 3);
		}
		break;
	default:;
	}
}

static bool LoadMaterialFile(Material& material, const char* filename)
{
	File file;
	if (!OpenFileRead(file, filename))
	{
		LogFatal("Could not load material \"%s\": file does not exist.", filename);
		return false;
	}

	char* text = (char*)malloc(file.size + 1);
	if (text == nullptr)
	{
		LogFatal("Could not load material \"%s\": out of memory.", filename);
		CloseFile(file);
		return false;
	}

	if (!ReadFileData(file, text) || file.size == 0)
	{
		LogFatal("Could not load material \"%s\": file reading error.", filename);
		free(text);
		CloseFile(file);
		return false;
	}
	text[file.size] = '\0';

	rapidxml::xml_document<> doc;
	doc.parse<0>(text);

	rapidxml::xml_node<>* root = doc.first_node("material");
	if (root == nullptr)
	{
		LogFatal("Could not load material \"%s\": invalid file format.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

	rapidxml::xml_node<>* node = root->first_node("technique");
	if (node == nullptr)
	{
		LogFatal("Could not load material \"%s\": no techniques specialized.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

	rapidxml::xml_attribute<>* attrib = node->first_attribute("name");
	if (attrib == nullptr)
	{
		LogFatal("Could not load material \"%s\": no technique name specialized.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

	Technique technique;
	if (!GetTechnique(technique, attrib->value()))
	{
		LogFatal("Could not load material \"%s\": technique loading error.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

	const ResourceIndex index = s_cache.handles.size();
	material.handle = s_cache.indices.insert(index);

	s_cache.by_name.emplace(filename, material.handle);
	s_cache.techniques.insert(technique);
	s_cache.handles.insert(material.handle);
	s_cache.refs.insert(1);
	s_cache.filenames.insert(filename);

	const char* name_str;
	const char* value_str;
	UniformPair uniform_value;
	for (node = root->first_node("parameter"); node != nullptr; node = node->next_sibling("parameter"))
	{
		attrib = node->first_attribute("name");
		if (attrib == nullptr)
		{
			LogFatal("Could not load material \"%s\": parameter does not have a name.", filename);
			ReleaseTechnique(technique);
			free(text);
			CloseFile(file);
			return false;
		}
		name_str = attrib->value();

		attrib = node->first_attribute("value");
		if (attrib == nullptr)
		{
			LogFatal("Could not load material \"%s\": parameter does not have a value.", filename);
			ReleaseTechnique(technique);
			free(text);
			CloseFile(file);
			return false;
		}
		value_str = attrib->value();

		uniform_value.uniform = GetUniform(name_str);
		uniform_value.data = (char*)malloc(GetUniformSize(uniform_value.uniform));
		ParseUniformParameter(uniform_value.data, value_str, GetUniformType(uniform_value.uniform));

		s_cache.uniforms.emplace(material.handle, uniform_value);
	}

	LogInfo("Material \"%s\" loaded.", filename);
	return true;
}

bool GetMaterial(Material& material, const char* filename)
{
	const string_hash filename_hash = filename;
	const auto it = s_cache.by_name.find(filename_hash);
	if (it != s_cache.by_name.end())
	{
		material.handle = it->second;
		++s_cache.refs[it->second];
		return true;
	}
	else
		return LoadMaterialFile(material, filename);
}

void ReleaseMaterial(Material material)
{
	const ResourceIndex index = s_cache.indices[material.handle];
	if (--s_cache.refs[index] == 0)
	{
		ReleaseTechnique(s_cache.techniques[index]);

		s_cache.by_name.erase(s_cache.filenames[index]);
		s_cache.indices.erase(material.handle);
		s_cache.uniforms.erase(material.handle);

		const ResourceIndex rebound = s_cache.handles.erase(index);
		s_cache.techniques.erase(index);
		s_cache.refs.erase(index);
		s_cache.filenames.erase(index);

		if (rebound != s_cache.handles.INVALID_KEY)
			s_cache.indices[s_cache.handles[index]] = index;
	}
}

bgfx::ProgramHandle UseMaterial(Material material)
{
	const ResourceIndex index = s_cache.indices[material.handle];

	auto [begin, end] = s_cache.uniforms.equal_range(index);
	for (; begin != end; ++begin)
		bgfx::setUniform(begin->second.uniform.handle, begin->second.data, 1);

	return GetTechniqueProgram(s_cache.techniques[index]);
}

bgfx::ProgramHandle UseMaterial(Material material, bgfx::Encoder* queue)
{
	const ResourceIndex index = s_cache.indices[material.handle];

	auto [begin, end] = s_cache.uniforms.equal_range(index);
	for (; begin != end; ++begin)
		queue->setUniform(begin->second.uniform.handle, begin->second.data, 1);

	return GetTechniqueProgram(s_cache.techniques[index]);
}
} // namespace A3D
