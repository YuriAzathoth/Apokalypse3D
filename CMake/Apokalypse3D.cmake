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
	SET (PLATFORMS_LIST glsl spirv essl dx9 dx11 msl)
	SET (ARGS_glsl --platform linux -p 120)
	SET (ARGS_spirv --platform linux -p spirv)
	SET (ARGS_essl --platform android)
	SET (ARGS_msl --platform osx -p metal)
	SET (ARGS_pssl --platform orbis -p pssl)
	SET (ARGS_dx9 --platform windows -p s_3_0)
	SET (ARGS_dx11 --platform windows -p s_5_0)
	IF (${TYPE} STREQUAL VERTEX)
		SET (SHADER_TYPE v)
	ELSEIF (${TYPE} STREQUAL FRAGMENT)
		SET (SHADER_TYPE f)
	ELSE ()
		MESSAGE (FATAL_ERROR "Unrecognized shader type ${TYPE}")
	ENDIF ()
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	FOREACH (PLATFORM ${PLATFORMS_LIST})
		SET (OUT "${DIR_OUT}/${PLATFORM}/${FILENAME}${FILEEXT}.shader")
		LIST (APPEND ${GENERATED_FILES} ${OUT})
		ADD_CUSTOM_COMMAND (
			OUTPUT ${OUT}
			COMMAND shaderc
				-f "${IN}"
				-o "${OUT}"
				-i "${CMAKE_SOURCE_DIR}/ThirdParty/bgfx/bgfx/src"
				--type ${SHADER_TYPE}
				--varyingdef "${FILEPATH}/${FILENAME}.var"
				${ARGS_${PLATFORM}}
			DEPENDS ${IN} shaderc
			WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/DataSource/Shaders"
		)
	ENDFOREACH ()
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
	CMAKE_PARSE_ARGUMENTS (ARG "" "PACKAGE;ROOT_DIR" "GENERATED;UNPACKED" ${ARGN})
	SET (RELATIVE_FILES_IN "")
	FOREACH (FILEPATH ${ARG_GENERATED})
		CMAKE_PATH (RELATIVE_PATH FILEPATH BASE_DIRECTORY "${ARG_ROOT_DIR}" OUTPUT_VARIABLE RELATIVE_FILE)
		LIST (APPEND RELATIVE_FILES_IN "${RELATIVE_FILE}")
	ENDFOREACH ()
	FOREACH (FILEPATH ${ARG_UNPACKED})
		CMAKE_PATH (RELATIVE_PATH FILEPATH BASE_DIRECTORY "${ARG_ROOT_DIR}" OUTPUT_VARIABLE RELATIVE_FILE)
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
	SET (${GENERATED_FILES} ${RESOURCES})
	FOREACH (FILENAME ${RESOURCES})
		FILE (RELATIVE_PATH FILEPATH ${DIR_IN} ${FILENAME})
		GET_FILENAME_COMPONENT (FILEPATH ${FILEPATH} DIRECTORY)
		FILE (COPY ${FILENAME} DESTINATION ${DIR_OUT}/${FILEPATH})
	ENDFOREACH ()
ENDMACRO ()

MACRO (ADD_GAME_LAUNCHER TARGET_NAME)
	FILE (GLOB SOURCE_FILES *.cpp)
	FILE (GLOB HEADER_FILES *.cpp)
	ADD_EXECUTABLE (${TARGET_NAME} ${SOURCE_FILES} ${HEADER_FILES})
	TARGET_LINK_LIBRARIES (${TARGET_NAME} PRIVATE Apokalypse3D)
ENDMACRO ()

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
