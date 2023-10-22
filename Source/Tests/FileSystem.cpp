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

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "IO/FileSystem.h"
#include "LogDisable.inl"

TEST_SUITE("FileSystem")
{
	TEST_CASE("Directory")
	{
		constexpr const char DIRECTORY[] = "./Test";

		bool res = A3D::MakeDir(DIRECTORY);
		REQUIRE(res);
		res = A3D::IsDirExists(DIRECTORY);
		REQUIRE(res);
		res = A3D::RemoveDir(DIRECTORY);
		REQUIRE(res);
		res = A3D::IsDirExists(DIRECTORY);
		REQUIRE(!res);
	}
	TEST_CASE("DirectoryRecursive")
	{
		constexpr const char DIRECTORY_HIGHEST[] = "./One/Two/Three/Four";
		constexpr const char DIRECTORY_LOWEST[] = "./One";

		bool res = A3D::MakeDir(DIRECTORY_HIGHEST);
		REQUIRE(res);
		res = A3D::IsDirExists(DIRECTORY_HIGHEST);
		REQUIRE(res);
		res = A3D::RemoveDir(DIRECTORY_LOWEST);
		REQUIRE(res);
		res = A3D::IsDirExists(DIRECTORY_LOWEST);
		REQUIRE(!res);
	}
}
