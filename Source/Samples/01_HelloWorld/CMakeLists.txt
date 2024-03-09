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

SET (TARGET_NAME 01_HelloWorld)

FILE (GLOB SOURCE_FILES *.cpp)
FILE (GLOB HEADER_FILES *.h)

# CXX extension to exlude from library sources.
ADD_EXECUTABLE (${TARGET_NAME} Launcher.cxx)
TARGET_LINK_LIBRARIES (${TARGET_NAME} PRIVATE Engine)

ADD_LIBRARY (${TARGET_NAME}Lib SHARED ${SOURCE_FILES} ${HEADER_FILES})
TARGET_LINK_LIBRARIES (${TARGET_NAME}Lib PRIVATE Engine)

ADD_DEPENDENCIES (${TARGET_NAME} ${TARGET_NAME}Lib)
