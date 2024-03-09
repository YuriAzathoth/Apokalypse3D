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

#ifndef IO_FILE_H
#define IO_FILE_H

#include <stdint.h>
#include <stdio.h>
#include "EngineAPI.h"

namespace A3D
{
class ILog;

class ENGINEAPI_EXPORT File
{
public:
	enum class Mode : uint8_t
	{
		READ,
		WRITE
	};

	enum class Type : uint8_t
	{
		BINARY,
		TEXT
	};

	explicit File(ILog* log);
	~File();

	bool Open(const char* filename, Mode mode, Type type);
	bool OpenSubfileRead(const char* filename, Type type, uint32_t size, uint32_t offset);
	void Close();

	bool ReadData(void* dst, uint32_t size);
	bool WriteData(const void* dst, uint32_t size);

	template <typename T>
	bool Read(T& dst)
	{
		return ReadData(&dst, sizeof(T));
	}

	template <typename T>
	bool Write(const T& dst)
	{
		return WriteData(&dst, sizeof(T));
	}

	uint32_t GetSize() const { return size_; }
	uint32_t GetOffset() const { return offset_; }

private:
	FILE* handler_;
	ILog* log_;
	uint32_t offset_;
	uint32_t size_;
};
} // namespace A3D

#endif // IO_FILE_H
