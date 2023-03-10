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

#include <bx/debug.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <mutex>
#include "Log.h"
#include "System/Platform.h"

#define BUFFER_SIZE 1024

namespace A3D
{
static std::mutex g_mutex;

static char* g_filename = nullptr;
static enum LogLevel g_level = LogLevel::APOKALYPSE_LOG_LEVEL;

inline static const char* GetDate() noexcept
{
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	char* str = asctime(lt);
	str[strlen(str) - 1] = '\0';
	return str;
}

bool LogInit(const char* filepath, LogLevel logLevel) noexcept
{
	Mkdir(filepath);
	char filename[BUFFER_SIZE];
	sprintf(filename, "%s/%s.log", filepath, GetDate());
	for (char* chr = filename; *chr; ++chr)
		if (*chr == ':' || *chr == ' ')
			*chr = '_';
	g_filename = (char*)malloc(strlen(filename) + 1);
	strcpy(g_filename, filename);
	g_level = logLevel;
	return true;
}

void LogDestroy() noexcept
{
	free(g_filename);
}

#ifdef APOKALYPSE_LOG_FILELINE
void LogWrite(LogLevel level, const char* file, unsigned line, const char* format, ...) noexcept
#else // APOKALYPSE_LOG_FILELINE
void LogWrite(LogLevel level, const char* format, ...) noexcept
#endif // APOKALYPSE_LOG_FILELINE
{
	if (level < g_level)
		return;

	const char* type;
	switch (level)
	{
	case LogLevel::TRACE:
		type = "TRACE";
		break;
	case LogLevel::DEBUG:
		type = "DEBUG";
		break;
	case LogLevel::INFO:
		type = "INFO";
		break;
	case LogLevel::WARNING:
		type = "WARNING";
		break;
	case LogLevel::ERROR:
		type = "ERROR";
		break;
	default:
		type = nullptr;
	}
	char message[BUFFER_SIZE];
	char* messagetop = message + sprintf(message, "%s: %s: ", GetDate(), type);

#ifndef APOKALYPSE_LOG_FILELINE
	messagetop += sprintf(messagetop, "FILE %s LINE %u: ", file, line);
#endif // APOKALYPSE_LOG_FILELINE

	va_list args;
	va_start(args, format);
	messagetop += vsprintf(messagetop, format, args);
	va_end(args);

	sprintf(messagetop, "\n");

	{
		std::lock_guard<std::mutex> lock(g_mutex);
		printf(message);
		if (g_filename)
		{
			FILE* file = fopen(g_filename, "a");
			fprintf(file, message);
			fclose(file);
		}
	}

#ifndef NDEBUG
	if (level >= LogLevel::ERROR)
		bx::debugBreak();
#else // NDEBUG
		std::terminate();
#endif // NDEBUG
}
} // namespace A3D
