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
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include <meshoptimizer/src/meshoptimizer.h>
#include <string.h>
#include "GeometryComponents.h"
#include "GraphicsResourceComponents.h"
#include "GraphicsResourceSystems.h"
#include "IO/Log.h"
#include "RendererComponents.h"

#define BUFFER_SIZE 256

using namespace A3D::Components::Geometry;
using namespace A3D::Components::Resource::Graphics;
using namespace A3D::Components::Str;

namespace bgfx
{
int32_t read(bx::ReaderI* _reader, bgfx::VertexLayout& _layout, bx::Error* _err);
}

static inline const char* get_shader_path()
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

static inline void get_program_name(char* program, const char* vertex, const char* fragment)
{
	const size_t vertex_name_size = strlen(vertex);
	const size_t fragment_name_size = strlen(fragment);
	const size_t program_name_size = vertex_name_size + fragment_name_size + 1;
	memcpy(program, vertex, vertex_name_size);
	memcpy(program + fragment_name_size + 1, fragment, fragment_name_size);
	program[fragment_name_size] = '|';
	program[program_name_size] = '\0';
}

static void release_image(void*, void* userData)
{
	bimg::imageFree((bimg::ImageContainer*)userData);
}

static constexpr const unsigned CHUNK_VERTEX_BUFFER = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
static constexpr const unsigned CHUNK_VERTEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
static constexpr const unsigned CHUNK_INDEX_BUFFER = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
static constexpr const unsigned CHUNK_INDEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
static constexpr const unsigned CHUNK_PRIMITIVE = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

namespace A3D
{
GraphicsResourceSystems::GraphicsResourceSystems(flecs::world& world)
{
	world.import<RendererComponents>();
	world.import<GeometryComponents>();
	world.import<GraphicsResourceComponents>();

	flecs::_::cpp_type<GraphicsResourceSystems>::id_explicit(world, 0, false);
	world.module<GraphicsResourceSystems>("A3D::Systems::Resource::Graphics");

	gpuProgramStorage_ = world.query_builder<>().term<const GpuProgramStorage>().build();
	meshStorage_ = world.query_builder<>().term<const MeshStorage>().build();
	shaderStorage_ = world.query_builder<>().term<const ShaderStorage>().build();
	textureStorage_ = world.query_builder<>().term<const TextureStorage>().build();

	findModelFile_ = world.system<const GetModelFile>("FindModel")
					.kind(flecs::OnLoad)
					.no_staging()
					.each([this](flecs::entity e, const GetModelFile& file)
	{
		flecs::world w = e.world();
		flecs::entity storage = meshStorage_.first();
		ecs_assert(storage != 0, -1, "MeshStorage component must be created before loading models.");
		flecs::entity cached = storage.lookup(file.filename.value);
		if (cached == 0)
		{
			w.defer_suspend();
			cached = w.entity(file.filename.value).child_of(storage).add<LoadModelFile>();
			w.defer_resume();
		}
		cached.add<SetAfterLoad>(e);
		e.remove<GetModelFile>();
	});

	findGpuProgram_ = world.system<const GetGpuProgram>("FindGpuProgram")
					.kind(flecs::OnLoad)
					.no_staging()
					.each([this](flecs::entity e, const GetGpuProgram& program)
	{
		// ProgramName = VertexName|FragmentName
		char program_name[BUFFER_SIZE];
		get_program_name(program_name, program.vertex.value, program.fragment.value);

		flecs::world w = e.world();
		flecs::entity storage = gpuProgramStorage_.first();
		ecs_assert(storage != 0, -1, "GpuProgramStorage must be created before loading GPU programs.");
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
			cached.add<LinkGpuProgram>().add<ShaderVertex>(v).add<ShaderFragment>(f);
		}
		cached.add<SetAfterLoad>(e);
		e.remove<GetGpuProgram>();
	});

	findShader_ = world.system<const GetShader>("FindShader")
					.kind(flecs::PostLoad)
					.no_staging()
					.each([this](flecs::entity e, const GetShader& file)
	{
		LogDebug("Finding compiled shader \"%s\"...", file.filename.value);
		flecs::world w = e.world();
		flecs::entity storage = shaderStorage_.first();
		ecs_assert(storage != 0, -1, "ShaderStorage component must be created before loading models.");
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

	findTexture_ = world.system<const GetTexture>("FindTexture")
					.kind(flecs::PostLoad)
					.no_staging()
					.each([this](flecs::entity e, const GetTexture& file)
	{
		LogTrace("Finding texture \"%s\"...", file.filename.value);
		flecs::world w = e.world();
		flecs::entity storage = textureStorage_.first();
		ecs_assert(storage != 0, -1, "TextureStorage component must be created before loading textures.");
		flecs::entity cached = storage.lookup(file.filename.value);
		if (cached == 0)
		{
			w.defer_suspend();
			cached = w.entity(file.filename.value).child_of(storage).add<LoadTextureFile>();
			w.defer_resume();
		}
		cached.add<SetAfterLoad>(e);
		e.remove<GetTexture>();
	});

	loadModelFile_ = world.system<>("LoadModelFile")
					.term<const LoadModelFile>()
					.kind(flecs::PostLoad)
					.multi_threaded()
					.each([this](flecs::entity e)
	{
		bx::DefaultAllocator alloc;
		bx::FileReader filereader;

		const char* filename = e.name();
		LogInfo("Loading model \"%s\"..", filename);
		flecs::world w = e.world();
		if (!bx::open(&filereader, filename))
		{
			LogError("Could not find model file \"%s\".", filename);
			e.destruct();
			return;
		}

		bx::Error err;
		Aabb aabb;
		MeshGroup group;
		MeshPrimitive prim;
		Obb obb;
		Sphere sphere;
		VertexLayout layout;
		const bgfx::Memory* mem;
		void* compressed;
		void* str;
		unsigned chunk;
		unsigned size;
		unsigned i;
		unsigned short len;
		unsigned short num;
		flecs::entity g;
		flecs::entity p;
		while (bx::read(&filereader, chunk, &err) == 4 && err.isOk())
		{
			switch (chunk)
			{
			case CHUNK_VERTEX_BUFFER:
				bx::read(&filereader, sphere.sphere, &err);
				bx::read(&filereader, aabb.aabb, &err);
				bx::read(&filereader, obb.obb, &err);
				bgfx::read(&filereader, layout.layout, &err);
				bx::read(&filereader, group.verticesCount, &err);
				mem = bgfx::alloc(group.verticesCount * layout.layout.getStride());
				bx::read(&filereader, mem->data, mem->size, &err);
				group.vbo = bgfx::createVertexBuffer(mem, layout.layout);
				break;
			case CHUNK_VERTEX_BUFFER_COMPRESSED:
				bx::read(&filereader, sphere.sphere, &err);
				bx::read(&filereader, aabb.aabb, &err);
				bx::read(&filereader, obb.obb, &err);
				bgfx::read(&filereader, layout.layout, &err);
				bx::read(&filereader, group.verticesCount, &err);
				mem = bgfx::alloc(group.verticesCount * layout.layout.getStride());
				bx::read(&filereader, size, &err);
				compressed = BX_ALLOC(&alloc, size);
				bx::read(&filereader, compressed, size, &err);
				meshopt_decodeVertexBuffer(mem->data,
										   group.verticesCount,
										   layout.layout.getStride(),
										   (uint8_t*)compressed,
										   size);
				BX_FREE(&alloc, compressed);
				group.vbo = bgfx::createVertexBuffer(mem, layout.layout);
				break;
			case CHUNK_INDEX_BUFFER:
				bx::read(&filereader, group.indicesCount, &err);
				mem = bgfx::alloc(group.indicesCount * sizeof(uint16_t));
				bx::read(&filereader, mem->data, mem->size, &err);
				group.ebo = bgfx::createIndexBuffer(mem);
				break;
			case CHUNK_INDEX_BUFFER_COMPRESSED:
				bx::read(&filereader, group.indicesCount, &err);
				mem = bgfx::alloc(group.indicesCount * sizeof(uint16_t));
				bx::read(&filereader, size, &err);
				compressed = BX_ALLOC(&alloc, size);
				bx::read(&filereader, compressed, size, &err);
				meshopt_decodeIndexBuffer(mem->data,
										  group.indicesCount,
										  sizeof(uint16_t),
										  (uint8_t*)compressed,
										  size);
				BX_FREE(&alloc, compressed);
				group.ebo = bgfx::createIndexBuffer(mem);
				break;
			case CHUNK_PRIMITIVE:
				bx::read(&filereader, len, &err);
				if (len)
				{
//					str = BX_ALLOC(&alloc, len);
//					BX_FREE(&alloc, str);
				}
				g = w.entity().child_of(e)
					.set<MeshGroup>(group)
					.set<Aabb>(aabb)
					.set<Obb>(obb)
					.set<Sphere>(sphere)
					.set<VertexLayout>(layout);
				bx::read(&filereader, num, &err);
				for (i = 0; i < num; ++i)
				{
					bx::read(&filereader, len, &err);
					if (len)
					{
//						str = BX_ALLOC(&alloc, len);
//						BX_FREE(&alloc, str);
					}
					bx::read(&filereader, prim.startIndex, &err);
					bx::read(&filereader, prim.indicesCount, &err);
					bx::read(&filereader, prim.startVertex, &err);
					bx::read(&filereader, prim.verticesCount, &err);
					bx::read(&filereader, sphere.sphere, &err);
					bx::read(&filereader, aabb.aabb, &err);
					bx::read(&filereader, obb.obb, &err);
					p = w.entity().child_of(g)
						.set<MeshPrimitive>(prim)
						.set<Aabb>(aabb)
						.set<Obb>(obb)
						.set<Sphere>(sphere);
				}
				break;
			default:
				LogError("Unsupported model file format: \"%s\".", filename);
				bx::close(&filereader);
				e.destruct();
				return;
			}
		}
		bx::close(&filereader);
		e.remove<LoadModelFile>();
		LogInfo("Model \"%s\" has been loaded.", filename);
	});

	loadShaderFile_ = world.system<>("LoadShaderFile")
					.term<const LoadShaderFile>()
					.kind(flecs::PostUpdate)
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

	loadTextureFile_ = world.system<>("LoadTextureFile")
					.term<const LoadTextureFile>()
					.kind(flecs::PostUpdate)
					.each([this](flecs::entity e)
	{
		const char* filename = e.name().c_str();
		LogDebug("Loading texture \"%s\"...", filename);

		bx::DefaultAllocator allocator;
		bx::FileReader filereader;

		if (!bx::open(&filereader, filename))
		{
			LogError("Could not load texture: file \"%s\" not found.", filename);
			e.destruct();
			return;
		}
		unsigned size = (unsigned)bx::getSize(&filereader);
		void* data = BX_ALLOC(&allocator, size);
		if (!data)
		{
			LogError("Could not load texture \"%s\": out of memory.", filename);
			e.destruct();
			return;
		}
		bx::read(&filereader, data, size, bx::ErrorAssert{});
		bx::close(&filereader);

		bimg::ImageContainer* img = bimg::imageParse(&allocator, data, size);
		if (!img)
		{
			LogError("Could not load texture \"%s\": invalid image format.", filename);
			e.destruct();
			return;
		}

		const bgfx::Memory* mem = bgfx::makeRef(img->m_data, img->m_size, release_image, img);
		BX_FREE(&allocator, data);

		bgfx::TextureHandle texture;
		if (img->m_cubeMap)
		{
			texture = bgfx::createTextureCube(static_cast<uint16_t>(img->m_width),
											  img->m_numMips > 1,
											  img->m_numLayers,
											  static_cast<bgfx::TextureFormat::Enum>(img->m_format),
											  0,
											  mem);
		}
		else if (img->m_depth > 1)
		{
			texture = bgfx::createTexture3D(static_cast<uint16_t>(img->m_width),
											static_cast<uint16_t>(img->m_height),
											static_cast<uint16_t>(img->m_depth),
											img->m_numMips > 1,
											static_cast<bgfx::TextureFormat::Enum>(img->m_format),
											0,
											mem);
		}
		else
		{
			texture = bgfx::createTexture2D(static_cast<uint16_t>(img->m_width),
											static_cast<uint16_t>(img->m_height),
											img->m_numMips > 1,
											img->m_numLayers,
											static_cast<bgfx::TextureFormat::Enum>(img->m_format),
											0,
											mem);
		}

		if (bgfx::isValid(texture))
		{
			LogInfo("Texture file \"%s\" has been loaded.", filename);
			e.set<Texture>({ texture }).remove<LoadTextureFile>();
		}
		else
		{
			LogError("Could not load texture \"%s\": failed to send texture to GPU.", filename);
			e.destruct();
		}
	});

	linkGpuProgram_ = world.system<>("LinkGpuProgram")
					.term<const ShaderVertex>(flecs::Wildcard)
					.term<const ShaderFragment>(flecs::Wildcard)
					.term<const LinkGpuProgram>()
					.kind(flecs::OnStore)
					.each([this](flecs::iter& it, size_t i)
	{
		LogDebug("Begin linking GPU program...");
		flecs::entity e = it.entity(i);
		flecs::id vp = it.pair(1);
		flecs::id fp = it.pair(2);
		flecs::entity v = vp.second();
		flecs::entity f = fp.second();
		ecs_assert(v != f, -1, "Could not link program to same vertex and fragment shader.");
		const Shader* vertex = v.get<Shader>();
		const Shader* fragment = f.get<Shader>();
		ecs_assert_var(vertex, -1, "Could not link program to not compiled vertex shader.");
		ecs_assert_var(fragment, -1, "Could not link program to not compiled fragment shader.");
		bgfx::ProgramHandle program = bgfx::createProgram(vertex->handle, fragment->handle);
		if (bgfx::isValid(program))
		{
			LogInfo("GPU program has been linked.");
			e.set<GpuProgram>({program}).remove<LinkGpuProgram>().remove(vp).remove(fp);
			v.destruct();
			f.destruct();
		}
		else
		{
			LogError("Failed to link GPU program.");
			e.destruct();
		}
	});

	setMeshAfterLoad_ = world.system<>("SetMeshAfterLoad")
						.term<const SetAfterLoad>(flecs::Wildcard)
						.term<const MeshStorage>().parent()
						.term<const LoadModelFile>().not_()
						.kind(flecs::OnStore)
						.multi_threaded()
						.each([](flecs::iter& it, size_t i)
	{
		LogTrace("Setting model after loaded...");
		flecs::world w = it.world();
		flecs::id pair = it.pair(1);
		flecs::entity cached = it.entity(i);
		flecs::entity target = pair.second();
		cached.children([&target, &w](flecs::entity child)
		{
			const MeshGroup* group = child.get<MeshGroup>();
			ecs_assert(group != nullptr, -1, "Could not set not loaded mesh.");
			w.entity().child_of(target).set<MeshGroup>(*group);
		});
		target.add<Model>();
		cached.remove(pair);
	});

	setShaderAfterLoad_ = world.system<const Shader>("SetShaderAfterLoad")
						.term<const SetAfterLoad>(flecs::Wildcard)
						.term<const ShaderStorage>().parent()
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

	setGpuProgramAfterLoad_ = world.system<const GpuProgram>("SetGpuProgramAfterLoad")
						.term<const SetAfterLoad>(flecs::Wildcard)
						.term<const GpuProgramStorage>().parent()
						.term<const LinkGpuProgram>().not_()
						.kind(flecs::OnStore)
						.multi_threaded()
						.each([](flecs::iter& it, size_t i, const GpuProgram& program)
	{
		LogTrace("Setting program after loaded...");
		flecs::id pair = it.pair(2);
		flecs::entity cached = it.entity(i);
		flecs::entity target = pair.second();
		target.set<GpuProgram>(program);
		cached.remove(pair);
	});

	setTextureAfterLoad_ = world.system<const Texture>("SetTextureAfterLoad")
						.term<const SetAfterLoad>(flecs::Wildcard)
						.term<const TextureStorage>().parent()
						.term<const LinkGpuProgram>().not_()
						.kind(flecs::OnStore)
						.multi_threaded()
						.each([](flecs::iter& it, size_t i, const Texture& texture)
	{
		flecs::id pair = it.pair(2);
		flecs::entity cached = it.entity(i);
		flecs::entity target = pair.second();
		target.set<Texture>(texture);
		cached.remove(pair);
	});

	destroyGpuProgram_ = world.observer<GpuProgram>("DestroyGpuProgram")
					.event(flecs::UnSet)
					.each([](flecs::entity e, GpuProgram& program)
	{
		bgfx::destroy(program.handle);
	});

	destroyMesh_ = world.observer<MeshGroup>("DestroyMeshGroup")
					.event(flecs::UnSet)
					.each([](flecs::entity e, MeshGroup& group)
	{
		bgfx::destroy(group.vbo);
		bgfx::destroy(group.ebo);
	});

	destroyShader_ = world.observer<Shader>("DestroyShader")
					.event(flecs::UnSet)
					.each([](flecs::entity e, Shader& shader)
	{
		bgfx::destroy(shader.handle);
	});

	destroyTexture_ = world.observer<Texture>("DestroyTexture")
					  .event(flecs::UnSet)
					  .each([](Texture& texture)
	{
		bgfx::destroy(texture.handle);
	});

	world.entity().add<GpuProgramStorage>();
	world.entity().add<MeshStorage>();
	world.entity().add<ShaderStorage>();
	world.entity().add<TextureStorage>();
}
} // namespace A3D
