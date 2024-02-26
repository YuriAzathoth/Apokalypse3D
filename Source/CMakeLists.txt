#
# Apokalypse3D - Fast and cache-friendly 3D game engine
# Copyright (C) 2022-2024 Yuriy Zinchenko
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

ADD_SUBDIRECTORY (ThirdParty)
ADD_SUBDIRECTORY (Tools)
ADD_SUBDIRECTORY (Engine)

IF (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
	OPTION (APOKALYPSE_SAMPLES "Enable building samples" ON)
	MARK_AS_ADVANCED (APOKALYPSE_SAMPLES)
	IF (APOKALYPSE_SAMPLES)
		ADD_SUBDIRECTORY (Samples)
	ENDIF ()

	IF (NOT EMSCRIPTEN)
		OPTION (APOKALYPSE_BENCHMARKS "Enable building benchmarks" ON)
		OPTION (APOKALYPSE_TESTS "Enable building tests" ON)
		MARK_AS_ADVANCED (APOKALYPSE_BENCHMARKS)
		MARK_AS_ADVANCED (APOKALYPSE_TESTS)
		IF (APOKALYPSE_BENCHMARKS)
			ADD_SUBDIRECTORY (Benchmarks)
		ENDIF ()
		IF (APOKALYPSE_TESTS)
			INCLUDE (CTest REQUIRED)
			ENABLE_TESTING ()
			ADD_SUBDIRECTORY (Tests)
		ENDIF ()
	ENDIF ()
ENDIF ()
