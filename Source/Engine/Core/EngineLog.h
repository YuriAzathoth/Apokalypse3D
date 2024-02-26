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

#ifndef CORE_ENGINE_LOG_H
#define CORE_ENGINE_LOG_H

#include "ILog.h"
#include "Global.h"

#define LogFatal(FORMAT, ...) A3D::GetGlobal()->log->Fatal(FORMAT, ##__VA_ARGS__)
#define LogError(FORMAT, ...) A3D::GetGlobal()->log->Error(FORMAT, ##__VA_ARGS__)
#define LogWarning(FORMAT, ...) A3D::GetGlobal()->log->Warning(FORMAT, ##__VA_ARGS__)
#define LogInfo(FORMAT, ...) A3D::GetGlobal()->log->Info(FORMAT, ##__VA_ARGS__)

#ifdef APOKALYPSE_LOG_DEBUG
#define LogDebug(FORMAT, ...) A3D::GetGlobal()->log->Debug(FORMAT, ##__VA_ARGS__)
#else // APOKALYPSE_LOG_DEBUG
#define LogDebug(FORMAT, ...)
#endif // APOKALYPSE_LOG_DEBUG

#ifdef APOKALYPSE_LOG_TRACE
#define LogTrace(FORMAT, ...) A3D::GetGlobal()->log->Trace(FORMAT, ##__VA_ARGS__)
#else // APOKALYPSE_LOG_TRACE
#define LogTrace(FORMAT, ...)
#endif // APOKALYPSE_LOG_TRACE

#ifdef APOKALYPSE_ASSERTIONS
#define Assert(CONDITION, FORMAT, ...) A3D::GetGlobal()->log->Assertion(CONDITION, ("Assertion \"" #CONDITION "\" failed: " FORMAT), ##__VA_ARGS__)
#else // APOKALYPSE_ASSERTIONS
#define Assert(CONDITION, FORMAT, ...)
#endif // APOKALYPSE_ASSERTIONS

#endif // CORE_ENGINE_LOG_H
