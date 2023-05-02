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
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include "IO/Log.h"
#include "Texture.h"

namespace A3D
{
static void ReleaseImage(void*, void* image)
{
	bimg::imageFree((bimg::ImageContainer*)image);
}

bool LoadTextureFromFile(bgfx::TextureHandle& texture, const char* filename)
{
	LogDebug("Loading texture \"%s\"...", filename);

	bx::DefaultAllocator allocator;
	bx::FileReader filereader;

	if (!bx::open(&filereader, filename))
	{
		LogFatal("Could not load texture: file \"%s\" not found.", filename);
		return false;
	}
	unsigned size = (unsigned)bx::getSize(&filereader);
	void* data = BX_ALLOC(&allocator, size);
	if (!data)
	{
		LogFatal("Could not load texture \"%s\": out of memory.", filename);
		bx::close(&filereader);
		return false;
	}
	bx::read(&filereader, data, size, bx::ErrorAssert{});
	bx::close(&filereader);

	bimg::ImageContainer* img = bimg::imageParse(&allocator, data, size);
	if (!img)
	{
		LogFatal("Could not load texture \"%s\": invalid image format.", filename);
		return false;
	}

	const bgfx::Memory* mem = bgfx::makeRef(img->m_data, img->m_size, ReleaseImage, img);
	BX_FREE(&allocator, data);

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
		return true;
	}
	else
	{
		LogFatal("Could not load texture \"%s\": failed to send texture to GPU.", filename);
		return false;
	}
}

void DestroyTexture(bgfx::TextureHandle& texture) { bgfx::destroy(texture); }
} // namespace A3D
