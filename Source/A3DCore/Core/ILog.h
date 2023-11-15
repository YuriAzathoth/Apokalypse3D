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

#ifndef CORE_ILOG_H
#define CORE_ILOG_H

#include <stdarg.h>
#include "A3DCoreAPI.h"

// Undef ERROR macro on windows platform to avoid errors
#ifdef ERROR
#undef ERROR
#endif // ERROR

namespace A3D
{
class A3DCOREAPI_EXPORT ILog
{
public:
	enum class Level
	{
		TRACE,
		DEBUG,
		INFO,
		WARNING,
		ERROR,
		FATAL
	};

	ILog();
	explicit ILog(Level level);
	virtual ~ILog() {}

	void Fatal(const char* format, ...);
	void Error(const char* format, ...);
	void Warning(const char* format, ...);
	void Info(const char* format, ...);

#ifdef APOKALYPSE_LOG_DEBUG
	void Debug(const char* format, ...);
#else // APOKALYPSE_LOG_DEBUG
	inline static void Debug(const char*, ...) {}
#endif // APOKALYPSE_LOG_DEBUG

#ifdef APOKALYPSE_LOG_TRACE
	void Trace(const char* format, ...);
#else // APOKALYPSE_LOG_TRACE
	inline static void Trace(const char*, ...) {}
#endif // APOKALYPSE_LOG_TRACE

#ifdef APOKALYPSE_ASSERTIONS
	void Assert(bool condition, const char* format, ...);
#else // APOKALYPSE_ASSERTIONS
	inline static void Assertion(bool, const char*, ...) {}
#endif // APOKALYPSE_ASSERTIONS

	void SetLogLevel(Level level) { level_ = level; }
	Level SetLogLevel() const { return level_; }

protected:
	virtual void Write(const char* message, Level level) = 0;

private:
	Level level_;

protected:
	static const char* GetDate();
	static const char* GetLevelName(Level level);
};
} // namespace A3D

#endif // CORE_ILOG_H
