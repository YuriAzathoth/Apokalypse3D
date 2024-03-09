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

#include <string.h>
#include "Core/ILog.h"
#include "PluginStorage.h"
#include "System/SharedLibrary.h"

#define BUFFER_SIZE 256

namespace A3D
{
static void DecorateLibraryFilename(char* real_filename, const char* filename);

PluginStorage::PluginStorage(IAllocator* alloc, ILog* log) :
	alloc_(alloc),
	log_(log)
{
}

PluginStorage::~PluginStorage()
{
	for (std::unique_ptr<IPlugin>& plugin : plugins_)
		plugin->Shutdown();
	plugins_.clear();

	for (void* lib : libraries_)
		A3D_CloseLibrary(lib);
	libraries_.clear();

	by_name_.clear();
	indices_.clear();
	filenames_.clear();
	handles_.clear();
}

bool PluginStorage::Load(const char* filename)
{
	char real_filename[BUFFER_SIZE];
	DecorateLibraryFilename(real_filename, filename);

	if (by_name_.find(real_filename) != by_name_.end())
	{
		log_->Error("Could not load plugin library \"%s\": plugin is already loaded.", real_filename);
		return false;
	}

	void* lib;
	std::unique_ptr<IPlugin> plugin;
	if (!LoadAndCreate(&lib, plugin, real_filename))
		return false;

	const PluginIndex index = plugins_.insert(std::move(plugin));
	libraries_.insert(lib);
	filenames_.emplace(real_filename);

	const PluginHandle handle = indices_.insert(index);
	by_name_.emplace(real_filename, handle);
	handles_.insert(handle);

	return true;
}

void PluginStorage::Release(const char* filename)
{
	char real_filename[BUFFER_SIZE];
	DecorateLibraryFilename(real_filename, filename);

	const auto by_name_it = by_name_.find(real_filename);
	if (by_name_it == by_name_.end())
		return;

	const PluginHandle handle = by_name_it->second;
	by_name_.erase(by_name_it);

	const PluginIndex index = indices_[handle];
	indices_.erase(handle);

	plugins_[index]->Shutdown();
	plugins_.erase(index);

	A3D_CloseLibrary(libraries_[index]);
	libraries_.erase(index);

	filenames_.erase(index);

	const PluginIndex rebound_index = handles_.erase(index);
	if (rebound_index != plugins_.INVALID_KEY)
		indices_[handles_[rebound_index]] = index;
}

bool PluginStorage::ReloadAll()
{
	for (std::unique_ptr<IPlugin>& plugin : plugins_)
		plugin->Shutdown();
	plugins_.clear();

	for (void* lib : libraries_)
		A3D_CloseLibrary(lib);
	libraries_.clear();

	by_name_.clear();
	indices_.clear();
	handles_.clear();

	void* lib;
	std::unique_ptr<IPlugin> plugin;
	PluginIndex index;
	PluginHandle handle;
	for (string& name : filenames_)
		if (LoadAndCreate(&lib, plugin, name.c_str()))
		{
			index = plugins_.emplace(std::move(plugin));
			libraries_.insert(lib);

			handle = indices_.insert(index);

			by_name_.emplace(name, handle);
			handles_.insert(handle);
		}
		else
		{
			for (std::unique_ptr<IPlugin>& _plugin : plugins_)
				_plugin->Shutdown();
			plugins_.clear();
			for (void* _lib : libraries_)
				A3D_CloseLibrary(_lib);
			return false;
		}

	return true;
}

bool PluginStorage::LoadAndCreate(void** library, std::unique_ptr<IPlugin>& plugin, const char* filename)
{
	void* lib = A3D_LoadLibrary(filename);
	if (lib == nullptr)
	{
		log_->Error("Could not load plugin library \"%s\": file not found.", filename);
		return false;
	}

	CreatePluginFN* CreatePlugin = reinterpret_cast<CreatePluginFN*>(A3D_GetLibrarySymbol(lib, "CreatePlugin"));
	if (CreatePlugin == nullptr)
	{
		log_->Error("Could not load plugin library \"%s\": function \"CreatePlugin\" does not exist.", filename);
		A3D_CloseLibrary(lib);
		return false;
	}

	plugin = (*CreatePlugin)();
	if (plugin.get() == nullptr)
	{
		log_->Error("Could not load plugin library \"%s\": plugin interface construction error.", filename);
		A3D_CloseLibrary(lib);
		return false;
	}

	if (!plugin->Initialize())
	{
		log_->Error("Could not load plugin library \"%s\": plugin initialisation error.", filename);
		A3D_CloseLibrary(lib);
		return false;
	}

	*library = lib;

	return true;
}

bool PluginStorage::PreUpdate(float elapsed_time)
{
	bool res = true;
	for (std::unique_ptr<IPlugin>& plugin : plugins_)
		res = res && plugin->PreUpdate(elapsed_time);
	return res;
}

bool PluginStorage::Update(float elapsed_time)
{
	bool res = true;
	for (std::unique_ptr<IPlugin>& plugin : plugins_)
		res = res && plugin->Update(elapsed_time);
	return res;
}

bool PluginStorage::PostUpdate(float elapsed_time)
{
	bool res = true;
	for (std::unique_ptr<IPlugin>& plugin : plugins_)
		res = res && plugin->PostUpdate(elapsed_time);
	return res;
}

void DecorateLibraryFilename(char* real_filename, const char* filename)
{
	strcpy(real_filename, "lib");
	strcat(real_filename, filename);
#ifndef NDEBUG
	strcat(real_filename, "_d");
#endif // NDEBUG
#ifdef __WIN32__
	strcat(real_filename, ".dll");
#else // __WIN32__
	strcat(real_filename, ".so");
#endif // __WIN32__
}
} // namespace A3D
