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

SET (TARGET_NAME Apokalypse3D)

FILE (GLOB_RECURSE HEADER_FILES *.h)
FILE (GLOB_RECURSE SOURCE_FILES *.cpp)

SET (GENERATED_FILES "")
COMPILE_SHADERS_DIR ("${CMAKE_SOURCE_DIR}/DataSource/Shaders" "${CMAKE_BINARY_DIR}/Data/Shaders")

ADD_LIBRARY (${TARGET_NAME} ${APOKALYPSE_LIB_TYPE} ${SOURCE_FILES} ${HEADER_FILES} ${GENERATED_FILES})

TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<INSTALL_INTERFACE:${DEST_INCLUDE_DIR}/${TARGET_TYPE}
)

TARGET_LINK_LIBRARIES (${TARGET_NAME} PRIVATE glm SDL2::SDL2-static bx bgfx)

INCLUDE (GenerateExportHeader REQUIRED)
GENERATE_EXPORT_HEADER (${TARGET_NAME} BASE_NAME "${TARGET_NAME}API" EXPORT_FILE_NAME "${TARGET_NAME}API.h")
INSTALL (FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}API.h" DESTINATION "${DEST_INCLUDE_DIR}/${TARGET_TYPE}")
