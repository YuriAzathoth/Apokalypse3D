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
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/file.h>
#include <bx/readerwriter.h>
#include "Core/EngineLog.h"
#include "IO/File.h"
#include "Texture.h"

namespace A3D
{
static void ReleaseImage(void*, void* image)
{
	bimg::imageFree((bimg::ImageContainer*)image);
}

bool LoadTextureFromFile(Texture& texture, const char* filename)
{
	LogDebug("Loading texture \"%s\"...", filename);

	File file;
	if (!OpenFileRead(file, filename))
	{
		LogFatal("Could not load texture: file \"%s\" not found.", filename);
		return false;
	}

	if (file.size == 0)
	{
		LogFatal("Could not load texture \"%s\": file is empty.", filename);
		CloseFile(file);
		return false;
	}

	const bgfx::Memory* mem = bgfx::alloc(file.size);

	if (!ReadFileData(file, mem->data))
	{
		LogFatal("Could not load texture \"%s\": file read error.", filename);
		CloseFile(file);
		return false;
	}

	CloseFile(file);

	bx::DefaultAllocator allocator;
	bimg::ImageContainer* img = bimg::imageParse(&allocator, mem->data, file.size);
	if (!img)
	{
		LogFatal("Could not load texture \"%s\": invalid image format.", filename);
		return false;
	}

	mem = bgfx::makeRef(img->m_data, img->m_size, ReleaseImage, img);

	if (img->m_cubeMap)
	{
		texture.handle = bgfx::createTextureCube(static_cast<uint16_t>(img->m_width),
						 img->m_numMips > 1,
						 img->m_numLayers,
						 static_cast<bgfx::TextureFormat::Enum>(img->m_format),
						 0,
						 mem);
	}
	else if (img->m_depth > 1)
	{
		texture.handle = bgfx::createTexture3D(static_cast<uint16_t>(img->m_width),
											   static_cast<uint16_t>(img->m_height),
											   static_cast<uint16_t>(img->m_depth),
											   img->m_numMips > 1,
											   static_cast<bgfx::TextureFormat::Enum>(img->m_format),
											   0,
											   mem);
	}
	else
	{
		texture.handle = bgfx::createTexture2D(static_cast<uint16_t>(img->m_width),
											   static_cast<uint16_t>(img->m_height),
											   img->m_numMips > 1,
											   img->m_numLayers,
											   static_cast<bgfx::TextureFormat::Enum>(img->m_format),
											   0,
											   mem);
	}

	if (bgfx::isValid(texture.handle))
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

void DestroyTexture(Texture& texture)
{
	bgfx::destroy(texture.handle);
}
} // namespace A3D
