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

#include "Graphics.h"

#ifdef __WIN32__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

void A3D_DisableHighDpi()
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
