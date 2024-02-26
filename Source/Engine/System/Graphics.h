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

#ifndef SYSTEM_GRAPHICS_H
#define SYSTEM_GRAPHICS_H

#ifdef __WIN32__
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
void A3D_DisableHighDpi();
#ifdef __cplusplus
}
#endif // __cplusplus
#else // __WIN32__
#define A3D_DisableHighDpi()
#endif // __WIN32__

#endif // SYSTEM_GRAPHICS_H
