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

#ifndef ENGINE_IPLUGIN_H
#define ENGINE_IPLUGIN_H

#include <memory>
#include "EngineAPI.h"

#define DECLARE_PLUGIN(CLASS) \
extern "C" std::unique_ptr<IPlugin> CreatePlugin() { return std::make_unique<CLASS>(); }

namespace A3D
{
class ENGINEAPI_EXPORT IPlugin
{
public:
	IPlugin() {}
	virtual ~IPlugin() {}

	virtual bool Initialize() = 0;
	virtual void Shutdown() = 0;
	virtual void Update(float time_elapsed) = 0;

private:
	IPlugin(const IPlugin&) = delete;
	void operator=(const IPlugin&) = delete;
};
} // namespace A3D

#endif // ENGINE_IPLUGIN_H
