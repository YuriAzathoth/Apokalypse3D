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
#include "GeometryComponents.h"
#include "ImageCacheComponents.h"
#include "ImageCacheSystems.h"
#include "ImageComponents.h"
#include "IO/AsyncLoaderComponents.h"
#include "IO/Log.h"
#include "RendererComponents.h"

#define BUFFER_SIZE 256

using namespace A3D::Components::AsyncLoader;
using namespace A3D::Components::Image;
using namespace A3D::Components::Cache::Image;
using namespace A3D::Components::Renderer;
using namespace A3D::Components::Str;

static void release_image(void*, void* userData)
{
	bimg::imageFree((bimg::ImageContainer*)userData);
}

namespace A3D
{
ImageCacheSystems::ImageCacheSystems(flecs::world& world)
{
	world.import<AsyncLoaderComponents>();
	world.import<ImageCacheComponents>();
	world.import<ImageComponents>();
	world.import<RendererComponents>();

	flecs::_::cpp_type<ImageCacheSystems>::id_explicit(world, 0, false);
	world.module<ImageCacheSystems>("A3D::Systems::Cache::Image");

	textures_ = world.query_builder<>().term<const TextureStorage>().build();

	findTexture_ = world.system<const GetTexture>("FindTexture")
					.kind(flecs::PostLoad)
					.no_staging()
					.each([this](flecs::entity e, const GetTexture& file)
	{
		LogTrace("Finding texture \"%s\"...", file.filename.value);
		flecs::world w = e.world();
		flecs::entity storage = textures_.first();
		ecs_assert(storage != 0, -1, "Textures storage component must be created before loading textures.");
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

	loadFile_ = world.system<>("LoadFile")
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
			bx::close(&filereader);
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

	setAfterLoad_ = world.system<const Texture>("SetAfterLoad")
					.term<const SetAfterLoad>(flecs::Wildcard)
					.term<const TextureStorage>().parent()
					.term<const LoadTextureFile>().not_()
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

	destroy_ = world.observer<Texture>("Destroy")
			   .term<const TextureStorage>().parent()
			   .event(flecs::UnSet)
			   .each([](Texture& texture)
	{
		bgfx::destroy(texture.handle);
	});

	world.entity().add<TextureStorage>();
}
} // namespace A3D
