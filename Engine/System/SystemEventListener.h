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

#ifndef SYSTEM_SYSTEM_EVENT_H
#define SYSTEM_SYSTEM_EVENT_H

#include <stdint.h>
#include <unordered_map>
#include "Apokalypse3DAPI.h"

union SDL_Event;

namespace A3D
{
using SystemEventCallbackFunction = void(const SDL_Event&, void*);

struct SystemEventCallback
{
	SystemEventCallbackFunction* func;
	void* data;
};

struct SystemEventListener
{
	std::unordered_multimap<uint32_t, SystemEventCallback> callbacks;
};

APOKALYPSE3DAPI_EXPORT bool CreateSystemEventListener();
APOKALYPSE3DAPI_EXPORT void DestroySystemEventListener();
APOKALYPSE3DAPI_EXPORT void BindSystemEvent(SystemEventListener& listener, uint32_t event, void(*cb)(const SDL_Event&, void*), void* data);
APOKALYPSE3DAPI_EXPORT uint32_t PollSystemEvents(const SystemEventListener& listener);
} // namespace A3D

#endif // SYSTEM_SYSTEM_EVENT_H
