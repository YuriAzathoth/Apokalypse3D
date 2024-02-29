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

#include "PluginStorage.h"
#include "System/SharedLibrary.h"

namespace A3D
{
PluginStorage::PluginStorage()
{
}

PluginStorage::~PluginStorage()
{
	for (auto& p : plugins_)
	{
		p.second.plugin->Shutdown();
		A3D_CloseLibrary(p.second.library);
	}
}

bool PluginStorage::LoadPlugin(const char* filename)
{
	if (plugins_.find(filename) != plugins_.end())
		return false;

	PluginRecord record;

	record.library = A3D_LoadLibrary(filename);
	if (record.library == nullptr)
		return false;

	using PluginConstructor = std::unique_ptr<IPlugin>(*)();
	PluginConstructor CreatePlugin = reinterpret_cast<PluginConstructor>(A3D_GetLibrarySymbol(record.library, "CreatePlugin"));
	if (CreatePlugin == nullptr)
		return false;

	record.plugin = (*CreatePlugin)();
	if (record.plugin.get() == nullptr)
		return false;

	if (!record.plugin->Initialize())
		return false;

	plugins_.emplace(filename, std::move(record));

	return true;
}

void PluginStorage::ReleasePlugin(const char* filename)
{
	const auto it = plugins_.find(filename);
	if (it != plugins_.end())
	{
		it->second.plugin->Shutdown();
		A3D_CloseLibrary(it->second.library);
		plugins_.erase(it);
	}
}
} // namespace A3D
