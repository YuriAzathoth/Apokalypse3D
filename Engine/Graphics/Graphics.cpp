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

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <stdio.h>
#include <string.h>
#include "Graphics.h"

Graphics::Graphics()
	: window_(nullptr)
	, context_(nullptr)
{
}

bool Graphics::Start(const char* title, int width, int height, int glMajor, int glMinor, bool vsync)
{
	if (SDL_InitSubSystem(SDL_INIT_VIDEO) < 0)
	{
		constexpr char TITLE[] = "Failed to init SDL video subsystem";
		const char* message = SDL_GetError();
		const size_t length = sizeof(TITLE) + strlen(message) + 4;
		errorString_ = (char*)malloc(length);
		sprintf(errorString_, "%s : %s", TITLE, message);
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, glMajor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, glMinor);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window_ = SDL_CreateWindow(title,
							   SDL_WINDOWPOS_CENTERED,
							   SDL_WINDOWPOS_CENTERED,
							   width,
							   height,
							   SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);
	if (!window_)
	{
		constexpr char TITLE[] = "Failed to create SDL window";
		const char* message = SDL_GetError();
		const size_t length = sizeof(TITLE) + strlen(message) + 4;
		errorString_ = (char*)malloc(length);
		sprintf(errorString_, "%s : %s", TITLE, message);
		return false;
	}

	context_ = SDL_GL_CreateContext(window_);
	if (!context_)
	{
		constexpr char TITLE[] = "Failed to create SDL OpenGL context";
		const char* message = SDL_GetError();
		const size_t length = sizeof(TITLE) + strlen(message) + 4;
		errorString_ = (char*)malloc(length);
		sprintf(errorString_, "%s : %s", TITLE, message);
		return false;
	}

	if (vsync && SDL_GL_SetSwapInterval(1) < 0)
	{
		constexpr char TITLE[] = "Failed to set VSync";
		const char* message = SDL_GetError();
		const size_t length = sizeof(TITLE) + strlen(message) + 4;
		errorString_ = (char*)malloc(length);
		sprintf(errorString_, "%s : %s", TITLE, message);
		return false;
	}

	glewExperimental = GL_TRUE;
	GLenum status = glewInit();
	if (status != GLEW_OK)
	{
		constexpr char TITLE[] = "Failed to initialize GLEW";
		const char* message = reinterpret_cast<const char*>(glewGetErrorString(status));
		const size_t length = sizeof(TITLE) + strlen(message) + 4;
		errorString_ = (char*)malloc(length);
		sprintf(errorString_, "%s : %s", TITLE, message);
		return false;
	}

	SDL_StartTextInput();

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glFrontFace(GL_CCW);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	return true;
}

void Graphics::Stop()
{
	if (errorString_)
	{
		free(errorString_);
		errorString_ = nullptr;
	}
	SDL_StopTextInput();
	if (context_)
	{
		SDL_GL_DeleteContext(context_);
		context_ = nullptr;
	}
	if (window_)
	{
		SDL_DestroyWindow(window_);
		window_ = nullptr;
	}
	SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

void Graphics::BeginFrame() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void Graphics::EndFrame() { SDL_GL_SwapWindow(window_); }
