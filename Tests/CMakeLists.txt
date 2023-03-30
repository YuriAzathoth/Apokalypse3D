#
# Apokalypse3D - Fast and cache-friendly 3D game engine
# Copyright (C) 2022-2023 Yuriy Zinchenko
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
#

FILE (GLOB SOURCE_FILES *.cpp)

FOREACH (TEST_FILE ${SOURCE_FILES})
	GET_FILENAME_COMPONENT (TEST_NAME ${TEST_FILE} NAME_WE)
	SET (TEST_TARGET_NAME Test_${TEST_NAME})
	ADD_EXECUTABLE (${TEST_TARGET_NAME} ${TEST_FILE})
	TARGET_LINK_LIBRARIES (${TEST_TARGET_NAME} PRIVATE Apokalypse3D doctest)
	TARGET_COMPILE_DEFINITIONS (${TEST_TARGET_NAME} PRIVATE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)
	ADD_TEST (NAME ${TEST_NAME} COMMAND ${TEST_TARGET_NAME} WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
ENDFOREACH ()
