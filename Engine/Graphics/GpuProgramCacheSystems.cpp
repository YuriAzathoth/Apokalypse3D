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

#include <bgfx/bgfx.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include "GpuProgramCacheComponents.h"
#include "GpuProgramCacheSystems.h"
#include "GpuProgramComponents.h"
#include "IO/AsyncLoaderComponents.h"
#include "IO/Log.h"

#define BUFFER_SIZE 256

using namespace A3D::Components::AsyncLoader;
using namespace A3D::Components::Cache::GpuProgram;
using namespace A3D::Components::GpuProgram;
using namespace A3D::Components::Str;

namespace A3D
{
inline static void get_program_name(char* program, const char* vertex, const char* fragment)
{
	const size_t vertex_name_size = strlen(vertex);
	const size_t fragment_name_size = strlen(fragment);
	const size_t program_name_size = vertex_name_size + fragment_name_size + 1;
	memcpy(program, vertex, vertex_name_size);
	memcpy(program + fragment_name_size + 1, fragment, fragment_name_size);
	program[fragment_name_size] = '|';
	program[program_name_size] = '\0';
}

inline static const char* get_shader_path()
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
		LogError("BGFX error: invalid renderer mode: %d.", bgfx::getRendererType());
		return nullptr;
	}
}

static void destroy_program(Program& program)
{
	bgfx::destroy(program.handle);
}

static void destroy_shader(Shader& shader)
{
	bgfx::destroy(shader.handle);
}

static void clear_programs(flecs::entity e)
{
	e.world().filter_builder<Program>()
		.term<const ProgramStorage>().parent()
		.build()
		.each(destroy_program);
}

static void clear_shaders(flecs::entity e)
{
	e.world().filter_builder<Shader>()
		.term<const ShaderStorage>().parent()
		.build()
		.each(destroy_shader);
}

GpuProgramCacheSystems::GpuProgramCacheSystems(flecs::world& world)
{
	world.import<AsyncLoaderComponents>();
	world.import<GpuProgramCacheComponents>();
	world.import<GpuProgramComponents>();

	flecs::_::cpp_type<GpuProgramCacheSystems>::id_explicit(world, 0, false);
	world.module<GpuProgramCacheSystems>("A3D::Systems::Cache::GpuProgram");

	programs_ = world.query_builder<>().term<const ProgramStorage>().build();
	shaders_ = world.query_builder<>().term<const ShaderStorage>().build();

	findProgram_ = world.system<const GetProgram>("FindProgram")
				   .kind(flecs::OnLoad)
				   .no_staging()
				   .each([this](flecs::entity e, const GetProgram& program)
	{
		// ProgramName = VertexName|FragmentName
		char program_name[BUFFER_SIZE];
		get_program_name(program_name, program.vertex.value, program.fragment.value);

		flecs::world w = e.world();
		flecs::entity storage = programs_.first();
		ecs_assert(storage != 0, -1, "GPU program storage must be created before loading programs.");
		flecs::entity cached = storage.lookup(program_name);
		if (cached == 0)
		{
			LogDebug("Loading shaders \"%s\" and \"%s\"...",
					 program.vertex.value,
					 program.fragment.value);
			w.defer_suspend();
			cached = w.entity(program_name).child_of(storage);
			w.defer_resume();
			flecs::entity v = w.entity().set<GetShader>({program.vertex.value});
			flecs::entity f = w.entity().set<GetShader>({program.fragment.value});
			cached.add<LinkProgram>().add<ShaderVertex>(v).add<ShaderFragment>(f);
		}
		cached.add<SetAfterLoad>(e);
		e.remove<GetProgram>();
	});

	findShader_ = world.system<const GetShader>("FindShader")
				  .kind(flecs::PostLoad)
				  .no_staging()
				  .each([this](flecs::entity e, const GetShader& file)
	{
		LogDebug("Finding compiled shader \"%s\"...", file.filename.value);
		flecs::world w = e.world();
		flecs::entity storage = shaders_.first();
		ecs_assert(storage != 0, -1, "Shader storage must be created before loading models.");
		flecs::entity cached = storage.lookup(file.filename.value);
		if (cached == 0)
		{
			w.defer_suspend();
			cached = w.entity(file.filename.value).child_of(storage).add<LoadShaderFile>();
			w.defer_resume();
		}
		cached.add<SetAfterLoad>(e);
		e.remove<GetShader>();
	});

	loadShaderFile_ = world.system<>("LoadShaderFile")
					.term<const LoadShaderFile>()
					.kind(flecs::PostUpdate)
					.multi_threaded()
					.each([this](flecs::entity e)
	{
		const char* filename = e.name().c_str();
		LogDebug("Loading shader \"%s\"...", filename);

		char filepath[BUFFER_SIZE];
		sprintf(filepath, "../Data/Shaders/%s/%s.shader", get_shader_path(), filename);

		FILE* file = fopen(filepath, "rb");
		if (!file)
		{
			LogError("Could not load shader: file \"%s\" not found.", filepath);
			e.destruct();
			return;
		}

		fseek(file, 0, SEEK_END);
		const unsigned size = static_cast<unsigned>(ftell(file));
		if (!size)
		{
			LogError("Could not load shader: file \"%s\" is empty.", filepath);
			fclose(file);
			e.destruct();
			return;
		}
		rewind(file);

		const bgfx::Memory* mem = bgfx::alloc(size + 1);
		fread(mem->data, 1, size, file);
		fclose(file);

		bgfx::ShaderHandle shader = bgfx::createShader(mem);
		if (bgfx::isValid(shader))
		{
			LogInfo("Shader \"%s\" has been loaded.", e.name().c_str());
			e.set<Shader>({shader}).remove<LoadShaderFile>();
		}
		else
		{
			LogError("Shader \"%s\" compilation error.", e.name().c_str());
			e.destruct();
		}
	});

	linkProgram_ = world.system<>("LinkProgram")
				   .term<const ShaderVertex>(flecs::Wildcard)
				   .term<const ShaderFragment>(flecs::Wildcard)
				   .term<const LinkProgram>()
				   .kind(flecs::OnStore)
				   .each([this](flecs::iter& it, size_t i)
	{
		flecs::entity e = it.entity(i);
		flecs::id vp = it.pair(1);
		flecs::id fp = it.pair(2);
		flecs::entity v = vp.second();
		flecs::entity f = fp.second();
		ecs_assert(v != f, -1, "Could not link program to same vertex and fragment shader.");
		const Shader* vertex = v.get<Shader>();
		const Shader* fragment = f.get<Shader>();
		if (vertex && fragment) // Shaders already compiled
		{
			LogDebug("Begin linking GPU program...");
			ecs_assert_var(vertex, -1, "Could not link program to not compiled vertex shader.");
			ecs_assert_var(fragment, -1, "Could not link program to not compiled fragment shader.");
			bgfx::ProgramHandle program = bgfx::createProgram(vertex->handle, fragment->handle);
			if (bgfx::isValid(program))
			{
				LogInfo("GPU program has been linked.");
				e.set<Program>({program}).remove<LinkProgram>().remove(vp).remove(fp);
			}
			else
			{
				LogError("Failed to link GPU program.");
				e.destruct();
			}
			v.destruct();
			f.destruct();
		}
	});

	setShaderAfterLoad_ = world.system<const Shader>("SetShaderAfterLoad")
						.term<const SetAfterLoad>(flecs::Wildcard)
						.term<const LoadShaderFile>().not_()
						.kind(flecs::PreStore)
						.multi_threaded()
						.each([](flecs::iter& it, size_t i, const Shader& shader)
	{
		LogDebug("Setting shader after loaded...");
		flecs::id pair = it.pair(2);
		flecs::entity cached = it.entity(i);
		flecs::entity target = pair.second();
		target.set<Shader>(shader);
		cached.remove(pair);
	});

	setProgramAfterLoad_ = world.system<const Program>("SetProgramAfterLoad")
						   .term<const SetAfterLoad>(flecs::Wildcard)
						   .term<const LinkProgram>().not_()
						   .kind(flecs::OnStore)
						   .multi_threaded()
						   .each([](flecs::iter& it, size_t i, const Program& program)
	{
		LogTrace("Setting program after loaded...");
		flecs::id pair = it.pair(2);
		flecs::entity cached = it.entity(i);
		flecs::entity target = pair.second();
		target.set<Program>(program);
		cached.remove(pair);
	});

	clearPrograms_ = world.observer<>("DestroyPrograms")
					  .term<const ProgramStorage>()
					  .event(flecs::UnSet)
					  .each(clear_programs);

	clearShaders_ = world.observer<>("DestroyShader")
					 .term<const ProgramStorage>()
					 .event(flecs::UnSet)
					 .each(clear_shaders);

	world.entity().add<ProgramStorage>();
	world.entity().add<ShaderStorage>();
}
} // namespace A3D
