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

#ifndef CORE_DEFAULT_LOG_H
#define CORE_DEFAULT_LOG_H

#include <mutex>
#include "EngineAPI.h"
#include "EngineConfig.h"
#include "ILog.h"

#define A3DCORE_LOG_FILENAME_LENGTH 256

namespace A3D
{
struct IAllocator;

class ENGINEAPI_EXPORT DefaultLog : public ILog
{
public:
	DefaultLog(const char* filepath = nullptr, const char* filename = nullptr);
	DefaultLog(Level level, const char* filepath = nullptr, const char* filename = nullptr);

protected:
	void Write(const char* message, Level level) override;

private:
	void Initialize(const char* filepath, const char* filename);

	char filename_[A3DCORE_LOG_FILENAME_LENGTH];
	std::mutex file_mutex_;

private:
	static std::mutex s_con_mutex;
};
} // namespace A3D

#endif // CORE_DEFAULT_LOG_H
