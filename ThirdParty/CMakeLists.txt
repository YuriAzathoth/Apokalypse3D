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

SET (BUILD_SHARED_LIBS OFF)
SET (BUILD_STATIC_LIBS OFF)
ADD_SUBDIRECTORY (GLM)

SET (SDL_SHARED OFF)
SET (SDL_STATIC ON)
ADD_SUBDIRECTORY (SDL)

SET (BGFX_BUILD_EXAMPLES OFF CACHE BOOL "")
SET (BGFX_OPENGL_VERSION 33 CACHE STRING "")
ADD_SUBDIRECTORY (bgfx)
