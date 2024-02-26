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

#include <stdint.h>
#include "Debug.h"

#ifdef __WIN32__
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif __WIN32__

void A3D_DebugBreak()
{
	int* int3 = (int*)3L;
	*int3 = 3;
}

void A3D_Sleep(int sec, int nanosec)
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
