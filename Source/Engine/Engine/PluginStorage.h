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

#ifndef ENGINE_PLUGIN_STORAGE_H
#define ENGINE_PLUGIN_STORAGE_H

#include <memory>
#include <unordered_map>
#include "Container/dense_map.h"
#include "Container/sparse_map.h"
#include "Container/string.h"
#include "EngineAPI.h"
#include "IPlugin.h"

namespace A3D
{
class IAllocator;
class ILog;

using PluginHandle = uint16_t;
using PluginIndex = uint16_t;

class ENGINEAPI_EXPORT PluginStorage
{
public:
	PluginStorage(IAllocator* alloc, ILog* log);
	~PluginStorage();

	bool Load(const char* filename);
	void Release(const char* filename);
	bool ReloadAll();

	bool PreUpdate(float elapsed_time);
	bool Update(float elapsed_time);
	bool PostUpdate(float elapsed_time);

private:
	bool LoadAndCreate(void** library, std::unique_ptr<IPlugin>& plugin, const char* filename);

	std::unordered_map<string, PluginHandle> by_name_;
	sparse_map<PluginHandle, PluginIndex> indices_;
	dense_map<PluginIndex, std::unique_ptr<IPlugin>> plugins_;
	dense_map<PluginIndex, void*> libraries_;
	dense_map<PluginIndex, string> filenames_;
	dense_map<PluginIndex, PluginHandle> handles_;

	IAllocator* alloc_;
	ILog* log_;
};
} // namespace A3D

#endif // ENGINE_PLUGIN_STORAGE_H
