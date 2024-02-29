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
#include <string>
#include <unordered_map>
#include "EngineAPI.h"
#include "IPlugin.h"

namespace A3D
{
class ENGINEAPI_EXPORT PluginStorage
{
public:
	PluginStorage();
	~PluginStorage();

	bool LoadPlugin(const char* filename);
	void ReleasePlugin(const char* filename);

private:
	struct PluginRecord
	{
		std::unique_ptr<IPlugin> plugin;
		void* library;
	};

	std::unordered_map<std::string, PluginRecord> plugins_;
};
} // namespace A3D

#endif // ENGINE_PLUGIN_STORAGE_H
