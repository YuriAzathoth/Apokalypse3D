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

#ifndef IO_LOG_H
#define IO_LOG_H

#include "A3DCoreAPI.h"
#include "A3DCoreConfig.h"

// Undef ERROR macro if defined in windows.h
#ifdef ERROR
#undef ERROR
#endif // ERROR

namespace A3D
{
enum class LogLevel
{
	TRACE,
	DEBUG,
	INFO,
	WARNING,
	ERROR,
	FATAL,
	NONE
};

A3DCOREAPI_EXPORT void SetLogLevel(LogLevel logLevel) noexcept;
A3DCOREAPI_EXPORT bool LogInit(const char* filepath, LogLevel logLevel) noexcept;
A3DCOREAPI_EXPORT void LogDestroy() noexcept;

#ifdef APOKALYPSE_LOG_FILELINE
A3DCOREAPI_EXPORT void LogWrite(LogLevel level, const char* file, unsigned line, const char* format, ...) noexcept;
#else // APOKALYPSE_LOG_FILELINE
A3DCOREAPI_EXPORT void LogWrite(LogLevel level, const char* format, ...) noexcept;
#endif // APOKALYPSE_LOG_FILELINE

#ifdef APOKALYPSE_LOG_FILELINE
#define LogFatal(FORMAT, ...) LogWrite(LogLevel::FATAL, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogError(FORMAT, ...) LogWrite(LogLevel::ERROR, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogWarning(FORMAT, ...) LogWrite(LogLevel::WARNING, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#define LogInfo(FORMAT, ...) LogWrite(LogLevel::INFO, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#else // APOKALYPSE_LOG_FILELINE
#define LogFatal(FORMAT, ...) LogWrite(LogLevel::FATAL, FORMAT, ##__VA_ARGS__)
#define LogError(FORMAT, ...) LogWrite(LogLevel::ERROR, FORMAT, ##__VA_ARGS__)
#define LogWarning(FORMAT, ...) LogWrite(LogLevel::WARNING, FORMAT, ##__VA_ARGS__)
#define LogInfo(FORMAT, ...) LogWrite(LogLevel::INFO, FORMAT, ##__VA_ARGS__)
#endif // APOKALYPSE_LOG_FILELINE

#ifdef APOKALYPSE_ASSERTIONS
#ifdef APOKALYPSE_LOG_FILELINE
#define Assert(CONT, FORMAT, ...) \
if (!(CONT)) \
{ \
	LogWrite(LogLevel::ERROR, __FILE__, __LINE__, FORMAT "%s:", ##__VA_ARGS__, #CONT); \
}
#else // APOKALYPSE_LOG_FILELINE
#define Assert(CONT, FORMAT, ...) \
if (!(CONT)) \
{ \
	LogWrite(LogLevel::ERROR, FORMAT "%s:", ##__VA_ARGS__, #CONT); \
}
#endif // APOKALYPSE_LOG_FILELINE
#else // APOKALYPSE_ASSERTIONS
#define Assert(...)
#endif // APOKALYPSE_ASSERTIONS

#ifdef APOKALYPSE_LOG_DEBUG
#ifdef APOKALYPSE_LOG_FILELINE
#define LogDebug(FORMAT, ...) LogWrite(LogLevel::DEBUG, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#else // APOKALYPSE_LOG_FILELINE
#define LogDebug(FORMAT, ...) LogWrite(LogLevel::DEBUG, FORMAT, ##__VA_ARGS__)
#endif // APOKALYPSE_LOG_FILELINE
#define LogDebugBody(BODY) { BODY }
#else // APOKALYPSE_LOG_DEBUG
#define LogDebug(...)
#define LogDebugBody(BODY)
#endif // APOKALYPSE_LOG_DEBUG

#ifdef APOKALYPSE_LOG_TRACE
#ifdef APOKALYPSE_LOG_FILELINE
#define LogTrace(FORMAT, ...) LogWrite(LogLevel::TRACE, __FILE__, __LINE__, FORMAT, ##__VA_ARGS__)
#else // APOKALYPSE_LOG_FILELINE
#define LogDebug(FORMAT, ...) LogWrite(LogLevel::TRACE, FORMAT, ##__VA_ARGS__)
#endif // APOKALYPSE_LOG_FILELINE
#define LogTraceBody(BODY) { BODY }
#else // APOKALYPSE_LOG_TRACE
#define LogTrace(...)
#define LogTraceBody(BODY)
#endif // APOKALYPSE_LOG_TRACE
} // namespace A3D

#endif // IO_LOG_H
