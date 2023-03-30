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

MACRO (SELECT_COMPILE_ARCHITECTURE)
	IF (NOT EMSCRIPTEN)
		SET (APOKALYPSE_VECTORISATION "SSE4" CACHE STRING "Engine vectorisation level")
		SET_PROPERTY (CACHE APOKALYPSE_VECTORISATION PROPERTY STRINGS "NONE" "SSE3" "SSE4" "AVX" "AVX2" "AVX512")
		IF (APOKALYPSE_VECTORISATION STREQUAL "NONE")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=x86-64")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:IA32")
			ENDIF ()
		ELSEIF (APOKALYPSE_VECTORISATION STREQUAL "SSE3")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=core2")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:SSE2")
			ENDIF ()
		ELSEIF (APOKALYPSE_VECTORISATION STREQUAL "SSE4")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=westmere")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:SSE2")
			ENDIF ()
		ELSEIF (APOKALYPSE_VECTORISATION STREQUAL "AVX")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=sandybridge")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:AVX")
			ENDIF ()
		ELSEIF (APOKALYPSE_VECTORISATION STREQUAL "AVX2")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=haswell")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:AVX2")
			ENDIF ()
		ELSEIF (APOKALYPSE_VECTORISATION STREQUAL "AVX512")
			IF (NOT MSVC)
				ADD_COMPILE_OPTIONS ("-march=cascadelake")
			ELSE ()
				ADD_COMPILE_OPTIONS ("/arch:AVX512")
			ENDIF ()
		ENDIF ()
	ENDIF ()
ENDMACRO ()
