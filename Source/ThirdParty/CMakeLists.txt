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
SET (BGFX_OPENGL_VERSION 21 CACHE STRING "")
IF (EMSCRIPTEN)
	SET (BGFX_CONFIG_MULTITHREADED OFF CACHE BOOL "")
ENDIF ()
ADD_SUBDIRECTORY (bgfx)

# Prevent MinGW from MSYS2 using basetsd.h instead of stdint.h, because
# that does not contain std types e.g. uint32_t, int64_t etc.
IF (MINGW)
	TARGET_COMPILE_DEFINITIONS (glslang PUBLIC MINGW_HAS_SECURE_API=0)
ENDIF ()

SET (CGLM_SHARED OFF)
SET (CGLM_STATIC ON)
ADD_SUBDIRECTORY (cglm)

ADD_SUBDIRECTORY (rapidxml)

IF (NOT EMSCRIPTEN)
	SET (SDL2_DISABLE_SDL2MAIN ON CACHE BOOL "")
	SET (SDL_SHARED ON CACHE BOOL "")
	SET (SDL_STATIC OFF CACHE BOOL "")
	ADD_SUBDIRECTORY (SDL)

	IF (APOKALYPSE_BENCHMARKS)
		SET_CXX_EXCEPTIONS (ON)
		SET_CXX_RTTI (ON)
		SET (CELERO_COMPILE_DYNAMIC_LIBRARIES OFF CACHE BOOL "")
		ADD_SUBDIRECTORY (Celero)
		SET_CXX_EXCEPTIONS (OFF)
		SET_CXX_RTTI (OFF)
	ENDIF ()

	IF (APOKALYPSE_TESTS)
		SET (DOCTEST_NO_INSTALL OFF CACHE BOOL "")
		ADD_SUBDIRECTORY (doctest)
		TARGET_COMPILE_DEFINITIONS (doctest INTERFACE DOCTEST_CONFIG_NO_EXCEPTIONS_BUT_WITH_ALL_ASSERTS)
	ENDIF ()
ENDIF ()
