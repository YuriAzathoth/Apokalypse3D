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
#include <stdio.h>
#include <string>
#include "doctest/doctest.h"
#include "IO/FileSystem.h"

#ifdef __WIN32__
#define NO_OUTPUT " > nul"
#else // __WIN32__
#define NO_OUTPUT " > /dev/null"
#endif // __WIN32__

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
	TEST_CASE("Packing and unpacking")
	{
		constexpr const char PACKAGE[] = "Test.pak";
		constexpr const char FILE1_NAME[] = "HW.txt";
		constexpr const char FILE1_CONTENT[] = "Hello, World!";
		constexpr const char FILE2_NAME[] = "BasicText.txt";
		constexpr const char FILE2_CONTENT[] = "The quick brown fox jumps over the lazy dog";
		constexpr const char PACKING_COMMAND[] = "packer p Test.pak HW.txt BasicText.txt" NO_OUTPUT;
		constexpr const char UNPACKING_COMMAND[] = "packer u Test.pak ." NO_OUTPUT;
		constexpr const size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];

		FILE* file = fopen(FILE1_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE1_CONTENT, sizeof(FILE1_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		file = fopen(FILE2_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE2_CONTENT, sizeof(FILE2_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		system(PACKING_COMMAND);

		remove(FILE1_NAME);
		remove(FILE2_NAME);

		system(UNPACKING_COMMAND);

		long size;

		file = fopen(FILE1_NAME, "rb");
		REQUIRE(file != nullptr);
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);
		REQUIRE(size == sizeof(FILE1_CONTENT) - 1);
		REQUIRE(fread(buffer, size, 1, file) == 1);
		buffer[size] = '\0';
		REQUIRE(!strcmp(FILE1_CONTENT, buffer));
		fclose(file);

		file = fopen(FILE2_NAME, "rb");
		REQUIRE(file != nullptr);
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);
		REQUIRE(size == sizeof(FILE2_CONTENT) - 1);
		REQUIRE(fread(buffer, size, 1, file) == 1);
		buffer[size] = '\0';
		REQUIRE(!strcmp(FILE2_CONTENT, buffer));
		fclose(file);

		remove(FILE1_NAME);
		remove(FILE2_NAME);
		remove(PACKAGE);
	}
	TEST_CASE("Packing and unpacking subfolder")
	{
		constexpr const char PACKAGE[] = "./Test.pak";
		constexpr const char TEST_DIRECTORY[] = "./Test";
		constexpr const char FILE1_NAME[] = "./Test/HW.txt";
		constexpr const char FILE1_CONTENT[] = "Hello, World!";
		constexpr const char FILE2_NAME[] = "./Test/BasicText.txt";
		constexpr const char FILE2_CONTENT[] = "The quick brown fox jumps over the lazy dog";
		constexpr const char PACKING_COMMAND[] = "packer p ./Test.pak ./Test/HW.txt ./Test/BasicText.txt" NO_OUTPUT;
		constexpr const char UNPACKING_COMMAND[] = "packer u ./Test.pak ." NO_OUTPUT;
		constexpr const size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];

		REQUIRE(A3D::MakeDir(TEST_DIRECTORY) == true);

		FILE* file = fopen(FILE1_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE1_CONTENT, sizeof(FILE1_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		file = fopen(FILE2_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE2_CONTENT, sizeof(FILE2_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		system(PACKING_COMMAND);

		remove(FILE1_NAME);
		remove(FILE2_NAME);

		REQUIRE(A3D::RemoveDir(TEST_DIRECTORY) == true);

		system(UNPACKING_COMMAND);

		long size;

		file = fopen(FILE1_NAME, "rb");
		REQUIRE(file != nullptr);
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);
		REQUIRE(size == sizeof(FILE1_CONTENT) - 1);
		REQUIRE(fread(buffer, size, 1, file) == 1);
		buffer[size] = '\0';
		REQUIRE(!strcmp(FILE1_CONTENT, buffer));
		fclose(file);

		file = fopen(FILE2_NAME, "rb");
		REQUIRE(file != nullptr);
		fseek(file, 0, SEEK_END);
		size = ftell(file);
		rewind(file);
		REQUIRE(size == sizeof(FILE2_CONTENT) - 1);
		REQUIRE(fread(buffer, size, 1, file) == 1);
		buffer[size] = '\0';
		REQUIRE(!strcmp(FILE2_CONTENT, buffer));
		fclose(file);

		remove(PACKAGE);
		REQUIRE(A3D::RemoveDir(TEST_DIRECTORY) == true);
	}
	TEST_CASE("Open packed file")
	{
		constexpr const char PACKAGE[] = "./Test.pak";
		constexpr const char FILE1_NAME[] = "./HW.txt";
		constexpr const char FILE1_NAME_PACKED[] = "./Test/HW.txt";
		constexpr const char FILE1_CONTENT[] = "Hello, World!";
		constexpr const char FILE2_NAME[] = "./BasicText.txt";
		constexpr const char FILE2_CONTENT[] = "The quick brown fox jumps over the lazy dog";
		constexpr const char PACKING_COMMAND[] = "packer p ./Test.pak ./HW.txt ./BasicText.txt" NO_OUTPUT;
		constexpr const size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];

		FILE* file = fopen(FILE1_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE1_CONTENT, sizeof(FILE1_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		file = fopen(FILE2_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE2_CONTENT, sizeof(FILE2_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		system(PACKING_COMMAND);

		remove(FILE1_NAME);
		remove(FILE2_NAME);

		struct A3D::File f;
		REQUIRE(A3D::OpenFileRead(f, FILE1_NAME_PACKED));
		REQUIRE(f.size == sizeof(FILE1_CONTENT) - 1);
		A3D::ReadFileData(f, buffer);
		buffer[f.size] = '\0';
		REQUIRE(!strcmp(FILE1_CONTENT, buffer));
		A3D::CloseFile(f);

		remove(PACKAGE);
	}
	TEST_CASE("Open packed file with subdirectories")
	{
		constexpr const char PACKAGE[] = "./Test.pak";
		constexpr const char TEST_DIRECTORY[] = "./Test";
		constexpr const char FILE1_NAME[] = "./Test/HW.txt";
		constexpr const char FILE1_CONTENT[] = "Hello, World!";
		constexpr const char FILE2_NAME[] = "./Test/BasicText.txt";
		constexpr const char FILE2_NAME_PACKED[] = "./Test/Test/BasicText.txt";
		constexpr const char FILE2_CONTENT[] = "The quick brown fox jumps over the lazy dog";
		constexpr const char PACKING_COMMAND[] = "packer p ./Test.pak ./Test/HW.txt ./Test/BasicText.txt" NO_OUTPUT;
		constexpr const size_t BUFFER_SIZE = 256;
		char buffer[BUFFER_SIZE];

		REQUIRE(A3D::MakeDir(TEST_DIRECTORY) == true);

		FILE* file = fopen(FILE1_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE1_CONTENT, sizeof(FILE1_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		file = fopen(FILE2_NAME, "wb");
		REQUIRE(file != nullptr);
		REQUIRE(fwrite(FILE2_CONTENT, sizeof(FILE2_CONTENT) - 1, 1, file) == 1);
		fclose(file);

		system(PACKING_COMMAND);

		remove(FILE1_NAME);
		remove(FILE2_NAME);

		REQUIRE(A3D::RemoveDir(TEST_DIRECTORY) == true);

		struct A3D::File f;
		REQUIRE(A3D::OpenFileRead(f, FILE2_NAME_PACKED));
		REQUIRE(f.size == sizeof(FILE2_CONTENT) - 1);
		A3D::ReadFileData(f, buffer);
		buffer[f.size] = '\0';
		REQUIRE(!strcmp(FILE2_CONTENT, buffer));
		A3D::CloseFile(f);

		remove(PACKAGE);
	}
}
