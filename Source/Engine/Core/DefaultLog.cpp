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
#include <string.h>
#include "DefaultLog.h"
#include "System/Debug.h"
#include "System/FileSystem.h"

#define BUFFER_SIZE 1024

namespace A3D
{
std::mutex DefaultLog::s_con_mutex;

DefaultLog::DefaultLog(const char* filepath, const char* filename)
{
	Initialize(filepath, filename);
}

DefaultLog::DefaultLog(Level level, const char* filepath, const char* filename) :
	ILog(level)
{
	Initialize(filepath, filename);
}

void DefaultLog::Write(const char* message, Level level)
{
	char full_message[BUFFER_SIZE];
	sprintf_s(full_message, "%s: %s: %s.\n", GetDate(), GetLevelName(level), message);

	{
		std::unique_lock<std::mutex> lock(s_con_mutex);
		puts(full_message);
	}

	if (filename_[0] != 0)
	{
		std::unique_lock<std::mutex> lock(file_mutex_);
		FILE* file = fopen(filename_, "w");
		if (file != nullptr)
		{
			fputs(full_message, file);
			fclose(file);
		}
	}

	if (level == Level::FATAL)
	{
#ifndef NDEBUG
		A3D_DebugBreak();
#endif // NDEBUG
		SetFatal();
	}
}

void DefaultLog::Initialize(const char* filepath, const char* filename)
{
	if (filename == nullptr)
	{
		filename_[0] = '\0';
		return;
	}

	const char* date = GetDate();
	const size_t filepath_size = strlen(filename) + strlen(date) + 2;
	sprintf_s(filename_, A3DCORE_LOG_FILENAME_LENGTH, "%s/%s/%s", filepath, date, filename);

	filename_[filepath_size] = '\0';
	A3D_Mkdir(filename_);
	filename_[filepath_size] = '/';

	FILE* file = fopen(filename_, "w");
	if (file != nullptr)
		fclose(file);
	else
	{
		fprintf(stderr, "ERROR: Could not create log file \"%s\".", filename);
		filename_[0] = '\0';
	}
}
} // namespace A3D
