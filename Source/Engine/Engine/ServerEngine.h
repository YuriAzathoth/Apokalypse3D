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

#ifndef ENGINE_SERVER_ENGINE_H
#define ENGINE_SERVER_ENGINE_H

#include "EngineAPI.h"

namespace A3D
{
class IAllocator;
class ILog;

class ENGINEAPI_EXPORT ServerEngine
{
public:
	ServerEngine(IAllocator* alloc, ILog* log);
	~ServerEngine();

	bool Initialize();
	void Shutdown();

	bool PreUpdate();
	bool Update();
	bool PostUpdate();

private:
	IAllocator* alloc_;
	ILog* log_;
};
} // namespace A3D

#endif // ENGINE_SERVER_ENGINE_H
