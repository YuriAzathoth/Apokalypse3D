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

IF (EMSCRIPTEN)
	SET_PROPERTY (GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS TRUE)
	SET (CMAKE_SHARED_LIBRARY_CREATE_C_FLAGS "-s SIDE_MODULE=1")
	SET (CMAKE_SHARED_LIBRARY_CREATE_CXX_FLAGS "-s SIDE_MODULE=1")
	SET (CMAKE_STRIP FALSE)
ENDIF ()

MACRO (COMPILE_MODEL IN DIR_OUT GENERATED_FILES)
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	SET (OUT "${DIR_OUT}/${FILENAME}.mdl")
	LIST (APPEND ${GENERATED_FILES} ${OUT})
	ADD_CUSTOM_COMMAND (
		OUTPUT ${OUT}
		COMMAND geometryc
			-f "${IN}"
			-o "${OUT}"
			--packnormal 1
			--packuv 1
			--tangent
			-c
		DEPENDS ${IN} geometryc
		WORKING_DIRECTORY ${DIRECTORY}
	)
ENDMACRO ()

MACRO (COMPILE_SHADER TYPE IN DIR_OUT GENERATED_FILES)
	SET (SUFFIX_PROFILE_120 gl)
	SET (SUFFIX_PROFILE_spirv15-12 vk)
	SET (SUFFIX_PROFILE_metal metal)
	SET (SUFFIX_PROFILE_100_es gles)
	SET (SUFFIX_PROFILE_s_3_0 dx9)
	SET (SUFFIX_PROFILE_s_5_0 dx11)
	IF (${TYPE} STREQUAL VERTEX)
		SET (SHADER_TYPE vertex)
	ELSEIF (${TYPE} STREQUAL FRAGMENT)
		SET (SHADER_TYPE fragment)
	ELSE ()
		MESSAGE (FATAL_ERROR "Unrecognized shader type ${TYPE}")
	ENDIF ()
	SET (PROFILES_OPENGL 120)
	SET (PROFILES_VULKAN spirv15-12)
	SET (PROFILES_DX s_3_0 s_5_0)
	SET (PROFILES_METAL metal)
	SET (PROFILES_OPENGLES 100_es)
	IF (WIN32)
		SET (PROFILES_LIST ${PROFILES_OPENGL} ${PROFILES_VULKAN} ${PROFILES_DX})
		SET (PLATFORM windows)
	ELSEIF (UNIX)
		SET (PROFILES_LIST ${PROFILES_OPENGL} ${PROFILES_VULKAN})
		IF (NOT APPLE)
			SET (PLATFORM linux)
		ELSE ()
			LIST (APPEND PROFILES_LIST ${PROFILES_METAL})
			SET (PLATFORM osx)
		ENDIF ()
	ELSEIF (ANDROID)
		SET (PROFILES_LIST ${PROFILES_OPENGLES} ${PROFILES_VULKAN})
		SET (PLATFORM android)
	ELSE ()
		MESSAGE (FATAL_ERROR "Unsupported platform!")
	ENDIF ()
	# TODO: Add asm.js, ios, and orbis
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	FOREACH (PROFILE ${PROFILES_LIST})
		SET (OUT "${DIR_OUT}/${SUFFIX_PROFILE_${PROFILE}}/${FILENAME}${FILEEXT}")
		LIST (APPEND ${GENERATED_FILES} ${OUT})
		ADD_CUSTOM_COMMAND (
			OUTPUT ${OUT}
			COMMAND shaderc
				-f "${IN}"
				-o "${OUT}"
				-i "${CMAKE_SOURCE_DIR}/Source/ThirdParty/bgfx/bgfx/src"
				--type ${SHADER_TYPE}
				--varyingdef "${FILEPATH}/${FILENAME}.var"
				--platform ${PLATFORM}
				-p ${PROFILE}
			DEPENDS ${IN} shaderc
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/DataSource/Shaders"
		)
	ENDFOREACH ()
	#~ FOREACH (PROFILE ${PROFILES_LIST})
		#~ MESSAGE ("${PROFILE}")
	#~ ENDFOREACH ()
	#~ MESSAGE (FATAL_ERROR EndDebug)
	#~ SET (PLATFORMS_LIST)
	#~ IF (WIN32)
		#~ LIST (APPEND PLATFORMS_LIST glsl spirv dx9 dx11)
	#~ ELSEIF (UNIX)
		#~ LIST (APPEND PLATFORMS_LIST glsl spirv)
		#~ IF (APPLE)
			#~ LIST (APPEND PLATFORMS_LIST msl)
		#~ ENDIF ()
	#~ ELSEIF (ANDROID)
		#~ LIST (APPEND PLATFORMS_LIST spirv msl)
	#~ ENDIF ()
	#~ SET (ARGS_glsl --platform linux -p 120)
	#~ SET (ARGS_spirv --platform linux -p spirv)
	#~ SET (ARGS_essl --platform android -p 300_es)
	#~ SET (ARGS_msl --platform osx -p metal)
	#~ SET (ARGS_pssl --platform orbis -p pssl)
	#~ SET (ARGS_dx9 --platform windows -p s_3_0)
	#~ SET (ARGS_dx11 --platform windows -p s_5_0)
	#~ IF (${TYPE} STREQUAL VERTEX)
		#~ SET (SHADER_TYPE vertex)
	#~ ELSEIF (${TYPE} STREQUAL FRAGMENT)
		#~ SET (SHADER_TYPE fragment)
	#~ ELSE ()
		#~ MESSAGE (FATAL_ERROR "Unrecognized shader type ${TYPE}")
	#~ ENDIF ()
	#~ GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	#~ GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	#~ GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	#~ FOREACH (PLATFORM ${PLATFORMS_LIST})
		#~ SET (OUT "${DIR_OUT}/${FILENAME}${FILEEXT}.${PLATFORM}")
		#~ LIST (APPEND ${GENERATED_FILES} ${OUT})
		#~ ADD_CUSTOM_COMMAND (
			#~ OUTPUT ${OUT}
			#~ COMMAND shaderc
				#~ -f "${IN}"
				#~ -o "${OUT}"
				#~ -i "${CMAKE_SOURCE_DIR}/Source/ThirdParty/bgfx/bgfx/src"
				#~ --type ${SHADER_TYPE}
				#~ --varyingdef "${FILEPATH}/${FILENAME}.var"
				#~ ${ARGS_${PLATFORM}}
			#~ DEPENDS ${IN} shaderc
			#~ WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/DataSource/Shaders"
		#~ )
	#~ ENDFOREACH ()
ENDMACRO ()

MACRO (COMPILE_TEXTURE IN DIR_OUT GENERATED_FILES)
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	STRING (FIND ${IN} "_n." SUF REVERSE)
	SET (ARGS "")
	IF (SUF STREQUAL "_n.")
		LIST (APPEND ARGS "-n")
	ENDIF ()
	SET (OUT "${DIR_OUT}/${FILENAME}.ktx")
	LIST (APPEND ${GENERATED_FILES} ${OUT})
	ADD_CUSTOM_COMMAND (
		OUTPUT ${OUT}
		COMMAND texturec
			-f "${IN}"
			-o "${OUT}"
			-m
			${ARGS}
		DEPENDS ${IN} texturec
		WORKING_DIRECTORY ${DIRECTORY}
	)
ENDMACRO ()

MACRO (PACK_FILES)
	CMAKE_PARSE_ARGUMENTS (ARG "" "PACKAGE;ROOT_DIR" "FILES" ${ARGN})
	SET (RELATIVE_FILES_IN "")
	FOREACH (FILEPATH ${ARG_FILES})
		CMAKE_PATH (RELATIVE_PATH FILEPATH
					BASE_DIRECTORY "${ARG_ROOT_DIR}"
					OUTPUT_VARIABLE RELATIVE_FILE)
		LIST (APPEND RELATIVE_FILES_IN "${RELATIVE_FILE}")
	ENDFOREACH ()
	ADD_CUSTOM_COMMAND (
		OUTPUT ${ARG_PACKAGE}
		COMMAND packer p ${ARG_PACKAGE} ${RELATIVE_FILES_IN}
		DEPENDS ${FILES_IN} packer
		WORKING_DIRECTORY ${ARG_ROOT_DIR}
	)
ENDMACRO ()

MACRO (COMPILE_MODELS_DIR DIR_IN DIR_OUT GENERATED_FILES)
	FILE (GLOB_RECURSE OBJ_FILES ${DIR_IN}/*.obj)
	FILE (GLOB_RECURSE GLB_FILES ${DIR_IN}/*.glb)
	FILE (GLOB_RECURSE GLTF_FILES ${DIR_IN}/*.gltf)
	SET (MODELS ${OBJ_FILES} ${GLB_FILES} ${GLTF_FILES})
	FOREACH (MODEL ${MODELS})
		COMPILE_MODEL (${MODEL} ${DIR_OUT} GENERATED_FILES)
	ENDFOREACH ()
ENDMACRO ()

MACRO (COMPILE_SHADERS_DIR DIR_IN DIR_OUT GENERATED_FILES)
	FILE (GLOB_RECURSE FRAGMENT_SHADERS ${DIR_IN}/*.frag)
	FILE (GLOB_RECURSE VERTEX_SHADERS ${DIR_IN}/*.vert)
	FOREACH (FILENAME ${FRAGMENT_SHADERS})
		COMPILE_SHADER (FRAGMENT ${FILENAME} ${DIR_OUT} GENERATED_FILES)
	ENDFOREACH ()
	FOREACH (FILENAME ${VERTEX_SHADERS})
		COMPILE_SHADER (VERTEX ${FILENAME} ${DIR_OUT} GENERATED_FILES)
	ENDFOREACH ()
ENDMACRO ()

MACRO (COMPILE_TEXTURES_DIR DIR_IN DIR_OUT GENERATED_FILES)
	FILE (GLOB_RECURSE TEXTURES ${DIR_IN}/*.*)
	FOREACH (FILENAME ${TEXTURES})
		COMPILE_TEXTURE (${FILENAME} ${DIR_OUT} GENERATED_FILES)
	ENDFOREACH ()
ENDMACRO ()

MACRO (COPY_RESOURCES_DIR DIR_IN DIR_OUT GENERATED_FILES)
	FILE (GLOB_RECURSE RESOURCES ${DIR_IN}/*.*)
	FOREACH (FILENAME_IN ${RESOURCES})
		FILE (RELATIVE_PATH FILENAME_REL_IN ${DIR_IN} ${FILENAME_IN})
		SET (FILENAME_OUT ${DIR_OUT}/${FILENAME_REL_IN})
		LIST (APPEND ${GENERATED_FILES} ${FILENAME_OUT})
		ADD_CUSTOM_COMMAND (OUTPUT ${FILENAME_OUT}
							COMMAND ${CMAKE_COMMAND}
							-E copy_if_different
							${FILENAME_IN}
							${FILENAME_OUT}
							DEPENDS ${FILENAME_IN}
		)
	ENDFOREACH ()
ENDMACRO ()

MACRO (ADD_GAME_LAUNCHER TARGET_NAME)
	FILE (GLOB SOURCE_FILES *.cpp)
	FILE (GLOB HEADER_FILES *.cpp)
	ADD_EXECUTABLE (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
	TARGET_LINK_LIBRARIES (${TARGET_NAME} PRIVATE Apokalypse3D)
ENDMACRO ()
