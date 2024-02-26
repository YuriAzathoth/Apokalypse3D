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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "EngineConfig.h"
#include "ILog.h"

#define BUFFER_SIZE 1024

namespace A3D
{
ILog::ILog() :
	level_(Level::APOKALYPSE_LOG_LEVEL)
{
}

ILog::ILog(Level level) :
	level_(level)
{
}

void ILog::Fatal(const char* format, ...)
{
	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::FATAL);
}

void ILog::Error(const char* format, ...)
{
	if (level_ > Level::ERROR)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::ERROR);
}

void ILog::Warning(const char* format, ...)
{
	if (level_ > Level::WARNING)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::WARNING);
}

void ILog::Info(const char* format, ...)
{
	if (level_ > Level::INFO)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::INFO);
}

#ifdef APOKALYPSE_LOG_DEBUG
void ILog::Debug(const char* format, ...)
{
	if (level_ > Level::DEBUG)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::DEBUG);
}
#endif // APOKALYPSE_LOG_DEBUG

#ifdef APOKALYPSE_LOG_TRACE
void ILog::Debug(const char* format, ...)
{
	if (level_ > Level::TRACE)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::TRACE);
}
#endif // APOKALYPSE_LOG_TRACE

#ifdef APOKALYPSE_ASSERTIONS
void ILog::Assert(bool condition, const char* format, ...)
{
	if (condition == true)
		return;

	char message[BUFFER_SIZE];

	va_list args;
	va_start(args, format);
	vsprintf_s(message, format, args);
	va_end(args);

	Write(message, Level::FATAL);
}
#endif // APOKALYPSE_ASSERTIONS

const char* ILog::GetDate()
{
	time_t t = time(nullptr);
	struct tm* lt = localtime(&t);
	char* str = asctime(lt);
	str[strlen(str) - 1] = '\0';
	return str;
}

const char* ILog::GetLevelName(Level level)
{
	switch (level)
	{
	case Level::TRACE:	return "TRACE";
	case Level::DEBUG:	return "DEBUG";
	case Level::INFO:	return "INFO";
	case Level::WARNING:return "WARNING";
	case Level::ERROR:	return "ERROR";
	case Level::FATAL:	return "FATAL ERROR";
	default:			return nullptr;
	}
}
} // namespace A3D
