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

SET (TARGET_NAME A3DCore)

FILE (GLOB_RECURSE HEADER_FILES *.h)
FILE (GLOB_RECURSE SOURCE_CXX_FILES *.cpp)
FILE (GLOB_RECURSE SOURCE_C_FILES *.c)

ADD_LIBRARY (${TARGET_NAME} ${APOKALYPSE_LIB_TYPE} ${SOURCE_CXX_FILES} ${SOURCE_C_FILES} ${HEADER_FILES})

TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<INSTALL_INTERFACE:${DEST_INCLUDE_DIR}/${TARGET_TYPE}
	PRIVATE
	${MESHOPTIMIZER_INCLUDE_DIR}
)

TARGET_LINK_LIBRARIES (${TARGET_NAME} PUBLIC cglm rapidxml)

INCLUDE (GenerateExportHeader REQUIRED)
GENERATE_EXPORT_HEADER (${TARGET_NAME} BASE_NAME "${TARGET_NAME}API" EXPORT_FILE_NAME "${TARGET_NAME}API.h")
INSTALL (FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}API.h" DESTINATION "${DEST_INCLUDE_DIR}/${TARGET_TYPE}")

CONFIGURE_FILE (Config.h.in ${TARGET_NAME}Config.h)

TARGET_SOURCES (${TARGET_NAME} PRIVATE ${TARGET_NAME}API.h ${TARGET_NAME}Config.h)
