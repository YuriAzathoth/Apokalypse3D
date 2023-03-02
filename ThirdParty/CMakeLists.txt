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

SET (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "")
SET (BGFX_OPENGL_VERSION 12 CACHE STRING "")
IF (EMSCRIPTEN)
	SET (BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "")
ENDIF ()
ADD_SUBDIRECTORY (bgfx)

SET (FLECS_STATIC OFF CACHE BOOL "")
ADD_SUBDIRECTORY (flecs)

SET (BUILD_SHARED_LIBS OFF)
SET (BUILD_STATIC_LIBS OFF)
ADD_SUBDIRECTORY (GLM)

IF (NOT EMSCRIPTEN)
	SET (SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "")
	SET (SDL_SHARED ON CACHE BOOL "")
	SET (SDL_STATIC OFF CACHE BOOL "")
	ADD_SUBDIRECTORY (SDL)

	IF (APOKALYPSE_BENCHMARKS)
		SET (CELERO_COMPILE_DYNAMIC_LIBRARIES OFF CACHE BOOL "")
		ADD_SUBDIRECTORY (Celero)
	ENDIF ()

	IF (APOKALYPSE_TESTS)
		SET (DOCTEST_NO_INSTALL OFF CACHE BOOL "")
		ADD_SUBDIRECTORY (doctest)
	ENDIF ()
ENDIF ()

ADD_LIBRARY (rapidxml INTERFACE)
TARGET_INCLUDE_DIRECTORIES (rapidxml PUBLIC INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/rapidxml/include
	$<INSTALL_INTERFACE:${DEST_INCLUDE_DIR}/rapidxml
)
