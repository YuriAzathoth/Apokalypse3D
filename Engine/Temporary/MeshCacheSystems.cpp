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
#include <meshoptimizer/src/meshoptimizer.h>
#include "Graphics/GeometryComponents.h"
#include "Graphics/MeshComponents.h"
#include "Graphics/RendererComponents.h"
#include "MeshCacheComponents.h"
#include "MeshCacheSystems.h"
#include "IO/Log.h"
#include "IO/SyncCacheComponents.h"

#define BUFFER_SIZE 256

using namespace A3D::Components::Cache::Mesh;
using namespace A3D::Components::Geometry;
using namespace A3D::Components::Mesh;
using namespace A3D::Components::Renderer;
using namespace A3D::Components::SyncCache;

namespace bgfx
{
int32_t read(bx::ReaderI* _reader, bgfx::VertexLayout& _layout, bx::Error* _err);
}

static constexpr const unsigned CHUNK_VERTEX_BUFFER = BX_MAKEFOURCC('V', 'B', ' ', 0x1);
static constexpr const unsigned CHUNK_VERTEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('V', 'B', 'C', 0x0);
static constexpr const unsigned CHUNK_INDEX_BUFFER = BX_MAKEFOURCC('I', 'B', ' ', 0x0);
static constexpr const unsigned CHUNK_INDEX_BUFFER_COMPRESSED = BX_MAKEFOURCC('I', 'B', 'C', 0x1);
static constexpr const unsigned CHUNK_PRIMITIVE = BX_MAKEFOURCC('P', 'R', 'I', 0x0);

static void destroy_mesh(MeshGroup& group)
{
	bgfx::destroy(group.vbo);
	bgfx::destroy(group.ebo);
}

static void clear_meshes(flecs::entity e)
{
	e.world().filter_builder<MeshGroup>()
		.term<const MeshStorage>().parent()
		.build()
		.each(destroy_mesh);
}

A3D::MeshCacheSystems::MeshCacheSystems(flecs::world& world)
{
	world.module<MeshCacheSystems>("A3D::Systems::Cache::Mesh");
	world.import<GeometryComponents>();
	world.import<MeshCacheComponents>();
	world.import<MeshComponents>();
	world.import<RendererComponents>();
	world.import<SyncCacheComponents>();

	findFile_ = world.system<const GetModelFile>("FindFile")
				.kind(flecs::OnLoad)
				.no_readonly()
				.each([](flecs::entity e, const GetModelFile& file)
	{
		flecs::world w = e.world();
		flecs::entity storage = w.singleton<MeshStorage>();
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

	loadFile_ = world.system<>("LoadFile")
				.term<const LoadModelFile>()
				.term<const Renderer>().singleton()
				.kind(flecs::PostLoad)
				.multi_threaded()
				.each([](flecs::entity e)
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

	setAfterLoad_ = world.system<>("SetAfterLoad")
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

	clear_ = world.observer<>("Clear")
			   .term<const MeshStorage>()
			   .event(flecs::UnSet)
			   .each(clear_meshes);

	world.add<MeshStorage>();
}
