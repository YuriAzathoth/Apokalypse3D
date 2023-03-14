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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <filesystem>
#include "Platform.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else // __WIN32__
#include <sys/stat.h>
#endif // __WIN32__

namespace A3D
{
FileType GetFileAttribute(const char* filepath)
{
#ifdef __WIN32__
	const DWORD attrib = GetFileAttributes(filepath);
	if (attrib & FILE_ATTRIBUTE_NORMAL)
		return FileType::FILE;
	else if (attrib & FILE_ATTRIBUTE_DIRECTORY)
		return FileType::DIRECTORY;
	else
		return FileType::NONE;
#else // __WIN32__
	struct stat sb;
	if (stat(filename, &sb) == 0)
		return FileType::NONE;
	else if (S_ISREG(sb.st_mode))
		return FileType::FILE;
	else if (S_ISDIR(sb.st_mode))
		return FileType::DIRECTORY;
	else
		return FileType::NONE;
#endif // __WIN32__
}

void Mkdir(const char* path)
{
#ifdef __WIN32__
	CreateDirectory(path, nullptr);
#else // __WIN32__
	mkdir(path, 0774);
#endif // __WIN32__
}

void Rmdir(const char* path)
{
#ifdef __WIN32__
	RemoveDirectory(path);
#else // __WIN32__
	rmdir(path);
#endif // __WIN32__
}

bool Rmrf(const char* path)
{
	// TODO
	return std::filesystem::remove_all(path);
}

void Sleep(int sec, int nanosec)
{
#ifdef __WIN32__
	LARGE_INTEGER sleep_time;
	sleep_time.QuadPart = -((int64_t)sec * 10000000 + (int64_t)nanosec / 100);
	HANDLE hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
	SetWaitableTimer(hTimer, &sleep_time, 0, NULL, NULL, FALSE);
	WaitForSingleObject(hTimer, INFINITE);
	CloseHandle(hTimer);
#else // __WIN32__
	struct timespec sleep_time;
	sleep_time.tv_sec = sec;
	sleep_time.tv_nsec = nsec;
	nanosleep(&sleep_time, NULL);
#endif // __WIN32__
}

#if __WIN32__
void DisableHighDpi()
{
	typedef enum D3_PROCESS_DPI_AWARENESS
	{
		D3_PROCESS_DPI_UNAWARE = 0,
		D3_PROCESS_SYSTEM_DPI_AWARE = 1,
		D3_PROCESS_PER_MONITOR_DPI_AWARE = 2
	} YQ2_PROCESS_DPI_AWARENESS;
	HRESULT(WINAPI *SetProcessDpiAwareness)(YQ2_PROCESS_DPI_AWARENESS dpiAwareness) = NULL;
	HINSTANCE shcoreDll = LoadLibrary("SHCORE.DLL");
	if (shcoreDll)
	{
		SetProcessDpiAwareness = (HRESULT(WINAPI*)(YQ2_PROCESS_DPI_AWARENESS))
								 GetProcAddress(shcoreDll, "SetProcessDpiAwareness");
		if (SetProcessDpiAwareness)
		{
			SetProcessDpiAwareness(D3_PROCESS_PER_MONITOR_DPI_AWARE);
			FreeLibrary(shcoreDll);
			return;
		}
	}
	BOOL(WINAPI *SetProcessDPIAware)(void) = NULL;
	HINSTANCE userDll = LoadLibrary("USER32.DLL");
	if (userDll)
	{
		SetProcessDPIAware = (BOOL(WINAPI*)(void))GetProcAddress(userDll, "SetProcessDPIAware");
		if (SetProcessDPIAware)
			SetProcessDPIAware();
		FreeLibrary(userDll);
	}
}
#endif // __WIN32__

} // namespace A3D
