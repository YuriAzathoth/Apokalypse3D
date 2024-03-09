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

#include <string.h>
#include "Application.h"
#include "ClientEngine.h"
#include "ServerEngine.h"

namespace A3D
{
Application::Application() :
	plugins_(&alloc_, &log_),
	server_(nullptr),
	client_(nullptr)
{
}

Application::~Application()
{
	Shutdown();
}

int Application::Run(const char* window_title, int argc, char** argv)
{
	{
		StartupOptions options{};
		if (!ParseArgs(options, argc, argv))
			return 1;
		if (!Initialize(window_title, options))
			return 1;
	}
	const bool res = MainLoop();
	Shutdown();
	return res ? 0 : 1;
}

bool Application::ParseArgs(StartupOptions& options, int argc, char** argv)
{
	const char* argchr;
	for (const char** arg = (const char**)(argv + 1); arg < (const char**)(argv + argc); ++arg)
	{
		for (argchr = *arg; *argchr != '\0'; ++argchr)
		{
			if (*argchr == '-')
			{
				++argchr;
				if (argchr[1] == '\0')
				{
					switch(argchr[0])
					{
					case 'h':
						options.flags |= FLAG_HEADLESS;
						break;
					default:
						log_.Error("Invalid startup argument: \"%s\".", *arg);
						return false;
					}
				}
				else if (!strcmp(argchr, "headless"))
					options.flags |= FLAG_HEADLESS;
				else
				{
					log_.Error("Invalid startup argument: \"%s\".", *arg);
					return false;
				}
			}
		}
	}

	return true;
}

bool Application::Initialize(const char* window_title, const StartupOptions& options)
{
	server_ = new ServerEngine(&alloc_, &log_);
	if ((options.flags & FLAG_HEADLESS) == 0)
		client_ = new ClientEngine(&alloc_, &log_);

	if (!server_->Initialize())
		return false;

	GraphicsConfig graphics_config{};
	if (!client_->Initialize(window_title, graphics_config))
		return false;

	return true;
}

void Application::Shutdown()
{
	if (client_)
	{
		client_->Shutdown();
		delete client_;
		client_ = nullptr;
	}
	if (server_)
	{
		server_->Shutdown();
		delete server_;
		server_ = nullptr;
	}
}

bool Application::MainLoop()
{
	if (client_ != nullptr && server_ != nullptr)
	{
		while (true)
		{
			plugins_.PreUpdate(0);
			if (!server_->PreUpdate())
				return false;
			if (!client_->PreUpdate())
				return false;

			plugins_.Update(0);
			if (!server_->Update())
				return false;
			if (!client_->Update())
				return false;

			plugins_.PostUpdate(0);
			if (!server_->PostUpdate())
				return false;
			if (!client_->PostUpdate())
				return false;
		}
	}
	else if (server_ != nullptr)
	{
		if (!server_->PreUpdate())
			return false;
		if (!server_->Update())
			return false;
		if (!server_->PostUpdate())
			return false;
	}

	return true;
}

bool Application::LoadMainPlugin(const char* filename)
{
	return plugins_.Load(filename);
}
} // namespace A3D
