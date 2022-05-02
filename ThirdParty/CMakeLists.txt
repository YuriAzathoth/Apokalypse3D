#
# Apokalypse3D - Fast and cache-friendly 3D game engine
# Copyright (C) 2022 Yuriy Zinchenko
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

ADD_SUBDIRECTORY (EASTL)

SET (FLECS_STATIC_LIBS OFF CACHE BOOL "")
ADD_SUBDIRECTORY (flecs)
IF (WIN32)
	TARGET_LINK_LIBRARIES(flecs PRIVATE wsock32 ws2_32)
ENDIF ()

SET (BUILD_SHARED_LIBS OFF)
SET (BUILD_STATIC_LIBS OFF)
ADD_SUBDIRECTORY (GLM)

SET (SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "")
SET (SDL_SHARED ON)
SET (SDL_STATIC OFF)
ADD_SUBDIRECTORY (SDL)

SET (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "")
SET (BGFX_OPENGL_VERSION 33 CACHE STRING "")
ADD_SUBDIRECTORY (bgfx)

IF (APOKALYPSE_BENCHMARKS)
	SET (CELERO_COMPILE_DYNAMIC_LIBRARIES OFF CACHE BOOL "")
	ADD_SUBDIRECTORY (Celero)
ENDIF ()

IF (APOKALYPSE_TESTS)
	SET (DOCTEST_NO_INSTALL OFF CACHE BOOL "")
	ADD_SUBDIRECTORY (doctest)
ENDIF ()

ADD_LIBRARY (rapidxml INTERFACE)
TARGET_INCLUDE_DIRECTORIES (rapidxml PUBLIC INTERFACE
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>/rapidxml
)
