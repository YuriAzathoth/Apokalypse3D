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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <string.h>
#include <doctest/doctest.h>
#include "Container/meta/database.h"
#include "Container/meta/database_builder.h"
#include "DebugAllocator.inl"

TEST_SUITE("Database Table")
{
	TEST_CASE("Idle empty columns")
	{
		using db_t = A3D::db::database_builder<
			uint32_t,
			A3D::meta::types_list_builder<>,
			A3D::meta::types_list_builder<>,
			A3D::meta::types_list_builder<>,
			A3D::meta::types_list_builder<>
		>::build;
		db_t db;
	}
}
