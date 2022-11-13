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

#include <alloca.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "platform.h"

#ifdef _WIN32
#include <windows.h>
#define MKDIR(DIR) CreateDirectory(DIR, NULL)
#else // _WIN32
#include <pthread.h>
#include <sys/stat.h>
#define MKDIR(DIR) mkdir(DIR, 0774)
#endif // _WIN32

void a3d_mkdir(const char* path)
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

a3d_thread_t a3d_thread_create(a3d_thread_func func, void* param)
{
#ifdef __WIN32__
	HANDLE* hThread = (HANDLE*)malloc(sizeof(HANDLE));
	*hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)func, param, 0, NULL);
	return (a3d_thread_t)hThread;
#else // __WIN32__
	pthread_t* thread = (pthread_t*)malloc(sizeof(pthread_t));
	*thread = pthread_create(thread, NULL, func, param);
	return (a3d_thread_t)thread;
#endif // __WIN32__
}

void a3d_thread_destroy(a3d_thread_t thread)
{
#ifdef __WIN32__
	HANDLE* hThread = (HANDLE*)thread;
	TerminateThread(hThread, 0);
	CloseHandle(*hThread);
	free(hThread);
#else // __WIN32__
#endif // __WIN32__
}

a3d_mutex_t a3d_mutex_create()
{
#ifdef __WIN32__
	CRITICAL_SECTION* pMutex = (CRITICAL_SECTION*)malloc(sizeof(CRITICAL_SECTION));
	InitializeCriticalSection(pMutex);
	return (a3d_mutex_t)pMutex;
#else // __WIN32__
	pthread_mutex_t* pMutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(pMutex, NULL);
	return (a3d_mutex_t)pMutex;
#endif // __WIN32__
}

void a3d_mutex_destroy(a3d_mutex_t mutex)
{
#ifdef __WIN32__
	CRITICAL_SECTION* pMutex = (CRITICAL_SECTION*)mutex;
	DeleteCriticalSection(pMutex);
	free(pMutex);
#else // __WIN32__
	pthread_mutex_t* pMutex = (pthread_mutex_t*)mutex;
	pthread_mutex_destroy(pMutex);
	free(pMutex);
#endif // __WIN32__
}

void a3d_mutex_lock(a3d_mutex_t mutex)
{
#ifdef __WIN32__
	EnterCriticalSection((CRITICAL_SECTION*)mutex);
#else // __WIN32__
	pthread_mutex_lock((pthread_mutex_t*)mutex);
#endif // __WIN32__
}

void a3d_mutex_unlock(a3d_mutex_t mutex)
{
#ifdef __WIN32__
	LeaveCriticalSection((CRITICAL_SECTION*)mutex);
#else // __WIN32__
	pthread_mutex_unlock((pthread_mutex_t*)mutex);
#endif // __WIN32__
}

void a3d_sleep(int sec, int nanosec)
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
void a3d_disable_high_dpi()
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
