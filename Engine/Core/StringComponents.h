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

#ifndef STRINGCOMPONENTS_H
#define STRINGCOMPONENTS_H

#include <flecs.h>
#include "Apokalypse3DAPI.h"

namespace A3D
{
namespace Components
{
namespace Str
{
struct StringView
{
	char* value;

	StringView();
	StringView(const char* _value);
	~StringView();
	StringView(const StringView& src);
	StringView(StringView&& src) noexcept;
	StringView& operator=(const char* _value);
	StringView& operator=(const StringView& src);
	StringView& operator=(StringView&& src) noexcept;
};

inline StringView::StringView() : value(nullptr) {}
inline StringView::StringView(const char* _value) : value(ecs_os_strdup(_value)) {}
inline StringView::~StringView() { ecs_os_free(value); }
inline StringView::StringView(const StringView& src) : value(ecs_os_strdup(src.value)) {}

inline StringView::StringView(StringView&& src) noexcept
	: value(src.value)
{
	src.value = nullptr;
}

inline StringView& StringView::operator=(const char* _value)
{
	ecs_os_free(value);
	value = ecs_os_strdup(_value);
	return *this;
}

inline StringView& StringView::operator=(const StringView& src)
{
	ecs_os_free(value);
	value = ecs_os_strdup(src.value);
	return *this;
}

inline StringView& StringView::operator=(StringView&& src) noexcept
{
	ecs_os_free(value);
	value = src.value;
	src.value = nullptr;
	return *this;
}
} // namespace Str
} // namespace Components

struct APOKALYPSE3DAPI_EXPORT StringComponents
{
	StringComponents(flecs::world& world);
	flecs::entity stringView_;
};
} // namespace A3D

#endif // STRING_H
