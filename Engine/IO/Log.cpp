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

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Core/platform.h"
#include "Log.h"

#define BUFFER_SIZE 1024

static a3d_mutex_t g_mutex = 0;

static char* g_filename = nullptr;
#ifdef NDEBUG
static enum LogLevel g_level = LogLevel::LOG_LEVEL_ERROR;
#else // NDEBUG
static enum LogLevel g_level = LogLevel::LOG_LEVEL_TRACE;
#endif // NDEBUG

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
	a3d_mkdir(filepath);
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

#ifdef NDEBUG
void LogWrite(LogLevel level, const char* format, ...) noexcept
#else // NDEBUG
void LogWrite(LogLevel level, const char* file, unsigned line, const char* format, ...) noexcept
#endif // NDEBUG
{
	if (level < g_level)
		return;

	const char* type;
	switch (level)
	{
	case LOG_LEVEL_TRACE:
		type = "TRACE";
		break;
	case LOG_LEVEL_DEBUG:
		type = "DEBUG";
		break;
	case LOG_LEVEL_INFO:
		type = "INFO";
		break;
	case LOG_LEVEL_WARNING:
		type = "WARNING";
		break;
	case LOG_LEVEL_ERROR:
		type = "ERROR";
		break;
	default:
		type = nullptr;
	}
	char message[BUFFER_SIZE];
	char* messagetop = message + sprintf(message, "%s: %s: ", GetDate(), type);

#ifndef NDEBUG
	messagetop += sprintf(messagetop, "FILE %s LINE %u: ", file, line);
#endif // NDEBUG

	va_list args;
	va_start(args, format);
	messagetop += vsprintf(messagetop, format, args);
	va_end(args);

	sprintf(messagetop, "\n");

	if (g_mutex == 0)
		g_mutex = a3d_mutex_create();

	a3d_mutex_lock(g_mutex);
	printf(message);
	if (g_filename)
	{
		FILE* file = fopen(g_filename, "a");
		fprintf(file, message);
		fclose(file);
	}
	a3d_mutex_unlock(g_mutex);
}
