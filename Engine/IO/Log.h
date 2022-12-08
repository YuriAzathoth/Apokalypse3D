/*
	Apokalypse3D - Fast and cache-friendly 3D game engine
	Copyright (C) 2022 Yuriy Zinchenko

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

#ifndef LOG_H
#define LOG_H

#include "Apokalypse3DAPI.h"

#define LOG_LEVEL_TRACE_ENABLED

#ifdef NDEBUG
#define LogError(FORMAT, ...) LogWrite(LOG_LEVEL_ERROR, FORMAT, ##__VA_ARGS__)
#define LogWarning(FORMAT, ...) LogWrite(LOG_LEVEL_WARNING, FORMAT, ##__VA_ARGS__)
#define LogInfo(FORMAT, ...) LogWrite(LOG_LEVEL_INFO, FORMAT, ##__VA_ARGS__)
#define LogDebug(FORMAT, ...)
#define Assert(...)
#define DebugFor(EXPR, BODY)
#define DebugIf(EXPR, BODY)
#else // NDEBUG
#define LogError(FORMAT, ...) LogWrite(LOG_LEVEL_ERROR, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogWarning(FORMAT, ...) LogWrite(LOG_LEVEL_WARNING, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogInfo(FORMAT, ...) LogWrite(LOG_LEVEL_INFO, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogDebug(FORMAT, ...) LogWrite(LOG_LEVEL_DEBUG, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define Assert(CONT, FORMAT, ...) \
if (!(CONT)) \
{ \
	LogWrite(LOG_LEVEL_ERROR, __FILE__, __LINE__, FORMAT "%s:", ##__VA_ARGS__, #CONT); \
}
#define DebugFor(EXPR, BODY) for EXPR BODY
#define DebugIf(EXPR, BODY) if (EXPR) BODY
#endif // NDEBUG

#if !defined(NDEBUG) && defined(LOG_LEVEL_TRACE_ENABLED)
#define LogTrace(FORMAT, ...) LogWrite(LOG_LEVEL_TRACE, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#else // !defined(NDEBUG) && defined(LOG_LEVEL_TRACE_ENABLED)
#define LogTrace(...)
#endif // !defined(NDEBUG) && defined(LOG_LEVEL_TRACE_ENABLED)

enum LogLevel
{
	LOG_LEVEL_TRACE,
	LOG_LEVEL_DEBUG,
	LOG_LEVEL_INFO,
	LOG_LEVEL_WARNING,
	LOG_LEVEL_ERROR,
	LOG_LEVEL_NONE
};

APOKALYPSE3DAPI_EXPORT bool LogInit(const char* filepath, LogLevel logLevel) noexcept;
APOKALYPSE3DAPI_EXPORT void LogDestroy() noexcept;

#ifdef NDEBUG
APOKALYPSE3DAPI_EXPORT void LogWrite(LogLevel level, const char* format, ...) noexcept;
#else // NDEBUG
APOKALYPSE3DAPI_EXPORT void LogWrite(LogLevel level, const char* file, unsigned line, const char* format, ...) noexcept;
#endif // NDEBUG

#endif // LOG_H
