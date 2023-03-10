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

#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Platform.h"

#ifdef _WIN32
#include <windows.h>
#define MKDIR(DIR) CreateDirectory(DIR, NULL)
#else // _WIN32
#include <sys/stat.h>
#define MKDIR(DIR) mkdir(DIR, 0774)
#endif // _WIN32

namespace A3D
{
void Mkdir(const char* path)
{
	char* buffer = (char*)alloca(strlen(path) + 1);
	strcpy(buffer, path);
	for (char* end = buffer; *end; ++end)
		if (*end == '/')
		{
			*end = '\0';
			MKDIR(buffer);
			*end = '/';
		}
	MKDIR(buffer);
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
	typedef enum D3_PROCESS_DPI_AWARENESS {
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
