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
#include <string.h>
#include <unordered_map>
#include "Common/Technique.h"
#include "Container/dense_map.h"
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
using TechniqueHandleType = decltype(bgfx::ProgramHandle::idx);
using TimerType = uint8_t;

struct TechniqueCache
{
	std::unordered_map<string_hash, TechniqueHandleType> by_name;
	std::unordered_map<TechniqueHandleType, ResourceIndex> indices;

	dense_map<ResourceIndex, TechniqueHandleType> render_passes;
	dense_map<ResourceIndex, RefsCount> refs;
	dense_map<ResourceIndex, string> filenames;
};

static TechniqueCache s_cache;

static bool LoadTechniqueFile(Technique& pass, const char* filename)
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

	rapidxml::xml_node<>* root = doc.first_node("technique");
	if (root == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": invalid file format.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

//	rapidxml::xml_node<>* node = root->first_node("shader");
//	if (node == nullptr)
//	{
//		LogFatal("Could not load render technique \"%s\": shaders are not specialized.", filename);
//		free(text);
//		CloseFile(file);
//		return false;
//	}

	char vertex_filename[BUFFER_SIZE];
	char fragment_filename[BUFFER_SIZE];

	rapidxml::xml_attribute<>* attrib = root->first_attribute("vs");
	if (attrib == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": vertex shader is not specialized.", filename);
		free(text);
		CloseFile(file);
		return false;
	}
	strcpy(vertex_filename, attrib->value());

	attrib = root->first_attribute("fs");
	if (attrib == nullptr)
	{
		LogFatal("Could not load render technique \"%s\": fragment shader is not specialized.", filename);
		free(text);
		CloseFile(file);
		return false;
	}
	strcpy(fragment_filename, attrib->value());

	free(text);

	Shader vertex;
	Shader fragment;

	if (!GetShader(vertex, vertex_filename))
	{
		LogFatal("Could not load render technique \"%s\": vertex shader loading error.", filename);
		free(text);
		CloseFile(file);
		return false;
	}
	if (!GetShader(fragment, fragment_filename))
	{
		LogFatal("Could not load render technique \"%s\": fragment shader loading error.", filename);
		free(text);
		CloseFile(file);
		return false;
	}

	pass.program = bgfx::createProgram(vertex.handle, fragment.handle);

	if (bgfx::isValid(pass.program))
	{
		LogInfo("Render pass \"%s\" loaded.", filename);
		return true;
	}
	else
	{
		LogFatal("Could not load render technique \"%s\": GPU program linking error.", filename);
		return false;
	}
}

bool GetTechnique(Technique& pass, const char* filename)
{
	const string_hash filename_hash = filename;
	const auto it = s_cache.by_name.find(filename_hash);
	if (it != s_cache.by_name.end())
	{
		pass.program.idx = it->second;
		++s_cache.refs[it->second];
		return true;
	}
	else if (LoadTechniqueFile(pass, filename))
	{
		s_cache.by_name.emplace(filename, pass.program.idx);
		s_cache.indices.emplace(pass.program.idx, s_cache.render_passes.size());

		s_cache.render_passes.insert(pass.program.idx);
		s_cache.refs.insert(1);
		s_cache.filenames.insert(filename);

		return true;
	}
	else
		return false;

//	GpuProgramParts key;
//	key.vertex_shader = vertex_filename;
//	key.fragment_shader = fragment_filename;
//
//	const auto it = s_programs_cache.by_name.find(key);
//	if (it != s_programs_cache.by_name.end())
//	{
//		++s_programs_cache.refs[s_programs_cache.indices[it->second]];
//		program.handle.idx = it->second;
//		return true;
//	}
//	else if (LinkProgram(program, vertex_filename, fragment_filename))
//	{
//		s_programs_cache.by_name.emplace(key, program.handle.idx);
//		s_programs_cache.indices.emplace(program.handle.idx, s_programs_cache.programs.size());
//
//		s_programs_cache.programs.insert(program.handle.idx);
//		s_programs_cache.refs.insert(1);
//		s_programs_cache.parts.insert(key);
//
//		return true;
//	}
//	else
//	{
//		LogFatal("Could not load GPU program with shaders \"%s\" and \"%s\" : linking error.", vertex_filename, fragment_filename);
//		return false;
//	}
}

void ReleaseTechnique(const Technique& pass)
{
//	const ResourceIndex index = s_programs_cache.indices[program.handle.idx];
//	if (--s_programs_cache.refs[index] == 0)
//	{
//		bgfx::destroy(program.handle);
//
//		s_programs_cache.by_name.erase(s_programs_cache.parts[index]);
//		s_programs_cache.indices.erase(program.handle.idx);
//
//		const ResourceIndex rebound = s_programs_cache.programs.erase(index);
//		s_programs_cache.parts.erase(index);
//		s_programs_cache.refs.erase(index);
//
//		if (rebound != s_programs_cache.programs.INVALID_KEY)
//			s_programs_cache.indices[s_programs_cache.programs[index]] = index;
//	}
}
} // namespace A3D
