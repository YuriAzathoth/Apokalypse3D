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

SET (TARGET_NAME Apokalypse3D)

FILE (GLOB_RECURSE HEADER_FILES *.h)
FILE (GLOB_RECURSE SOURCE_CXX_FILES *.cpp)
FILE (GLOB_RECURSE SOURCE_C_FILES *.c)

SET (GENERATED_FILES "")
COMPILE_MODELS_DIR ("${CMAKE_SOURCE_DIR}/DataSource/Models" "${CMAKE_BINARY_DIR}/Data/Models")
COMPILE_SHADERS_DIR ("${CMAKE_SOURCE_DIR}/DataSource/Shaders" "${CMAKE_BINARY_DIR}/Data/Shaders")
COMPILE_TEXTURES_DIR ("${CMAKE_SOURCE_DIR}/DataSource/Textures" "${CMAKE_BINARY_DIR}/Data/Textures")
COPY_RESOURCES_DIR ("${CMAKE_SOURCE_DIR}/Data" "${CMAKE_BINARY_DIR}/Data")

INCLUDE (${CMAKE_SOURCE_DIR}/ThirdParty/bgfx/cmake/bgfx/3rdparty/meshoptimizer.cmake REQUIRED)
ADD_LIBRARY (${TARGET_NAME} ${APOKALYPSE_LIB_TYPE} ${SOURCE_CXX_FILES} ${SOURCE_C_FILES} ${HEADER_FILES} ${MESHOPTIMIZER_SOURCES} ${GENERATED_FILES})

TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME} PUBLIC
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
	$<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
	$<INSTALL_INTERFACE:${DEST_INCLUDE_DIR}/${TARGET_TYPE}
	PRIVATE
	${MESHOPTIMIZER_INCLUDE_DIR}
)

IF (EMSCRIPTEN)
	SET (COMMON_FLAGS "-s USE_SDL=2")
	SET_TARGET_PROPERTIES (${TARGET_NAME} PROPERTIES CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COMMON_FLAGS}")
	SET_TARGET_PROPERTIES (${TARGET_NAME} PROPERTIES C_FLAGS "${CMAKE_C_FLAGS} ${COMMON_FLAGS}")
	SET_TARGET_PROPERTIES (${TARGET_NAME} PROPERTIES EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COMMON_FLAGS}")
ELSE ()
	SET (SDL2_LIBRARIES SDL3::SDL3)
ENDIF ()

TARGET_INCLUDE_DIRECTORIES (${TARGET_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
TARGET_LINK_LIBRARIES (${TARGET_NAME} PUBLIC bx bgfx bimg bimg_decode flecs::flecs glm rapidxml ${SDL2_LIBRARIES})

INCLUDE (GenerateExportHeader REQUIRED)
GENERATE_EXPORT_HEADER (${TARGET_NAME} BASE_NAME "${TARGET_NAME}API" EXPORT_FILE_NAME "${TARGET_NAME}API.h")
INSTALL (FILES "${CMAKE_CURRENT_BINARY_DIR}/${TARGET_NAME}API.h" DESTINATION "${DEST_INCLUDE_DIR}/${TARGET_TYPE}")

CONFIGURE_FILE (Config.h.in ${TARGET_NAME}Config.h)

TARGET_SOURCES (${TARGET_NAME} PRIVATE ${TARGET_NAME}API.h ${TARGET_NAME}Config.h)
