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

#ifndef GRAPHICS_H
#define GRAPHICS_H

struct SDL_Window;
typedef void* SDL_GL_Context;

class Graphics
{
public:
	Graphics();
	~Graphics() { Stop(); }

	bool Start(const char* title, int width, int height, int glMajor, int glMinor, bool vsync);
	void Stop();

	void BeginFrame();
	void EndFrame();

	const char* const GetErrorString() const noexcept { return errorString_; }

private:
	SDL_Window* window_;
	SDL_GL_Context context_;
	char* errorString_;
};

#endif // COORDINATOR_H
