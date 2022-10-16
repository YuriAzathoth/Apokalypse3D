MACRO (COMPILE_MODEL IN DIR_OUT)
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	SET (OUT "${DIR_OUT}/${FILENAME}.mdl")
	LIST (APPEND GENERATED_FILES ${OUT})
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

MACRO (COMPILE_SHADER TYPE IN DIR_OUT)
	SET (PLATFORMS_LIST glsl spirv essl dx9 dx11 msl)
	SET (ARGS_glsl --platform linux -p 120)
	SET (ARGS_spirv --platform linux -p spirv)
	SET (ARGS_essl --platform android)
	SET (ARGS_msl --platform osx -p metal)
	SET (ARGS_pssl --platform orbis -p pssl)
	IF (${TYPE} STREQUAL VERTEX)
		SET (ARGS_dx9 --platform windows -p vs_3_0 -O 3)
		SET (ARGS_dx11 --platform windows -p vs_5_0 -O 3)
		SET (SHADER_TYPE v)
	ELSEIF (${TYPE} STREQUAL FRAGMENT)
		SET (ARGS_dx9 --platform windows -p ps_3_0 -O 3)
		SET (ARGS_dx11 --platform windows -p ps_5_0 -O 3)
		SET (SHADER_TYPE f)
	ELSE ()
		MESSAGE (FATAL_ERROR "Unrecognized shader type ${TYPE}")
	ENDIF ()
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	FOREACH (PLATFORM ${PLATFORMS_LIST})
		SET (OUT "${DIR_OUT}/${PLATFORM}/${FILENAME}${FILEEXT}.shader")
		LIST (APPEND GENERATED_FILES ${OUT})
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

MACRO (COMPILE_TEXTURE IN DIR_OUT)
	GET_FILENAME_COMPONENT (FILEPATH ${IN} DIRECTORY)
	GET_FILENAME_COMPONENT (FILENAME ${IN} NAME_WLE)
	GET_FILENAME_COMPONENT (FILEEXT ${IN} EXT)
	STRING (FIND ${IN} "_n." SUF REVERSE)
	SET (ARGS "")
	IF (SUF STREQUAL "_n.")
		LIST (APPEND "-n")
	ENDIF ()
	SET (OUT "${DIR_OUT}/${FILENAME}.ktx")
	LIST (APPEND GENERATED_FILES ${OUT})
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

MACRO (COMPILE_MODELS_DIR DIR_IN DIR_OUT)
	FILE (GLOB_RECURSE OBJ_FILES ${DIR_IN}/*.obj)
	FILE (GLOB_RECURSE GLB_FILES ${DIR_IN}/*.glb)
	FILE (GLOB_RECURSE GLTF_FILES ${DIR_IN}/*.gltf)
	SET (MODELS ${OBJ_FILES} ${GLB_FILES} ${GLTF_FILES})
	FOREACH (MODEL ${MODELS})
		COMPILE_MODEL (${MODEL} ${DIR_OUT})
	ENDFOREACH ()
ENDMACRO ()

MACRO (COMPILE_SHADERS_DIR DIR_IN DIR_OUT)
	FILE (GLOB_RECURSE FRAGMENT_SHADERS ${DIR_IN}/*.frag)
	FILE (GLOB_RECURSE VERTEX_SHADERS ${DIR_IN}/*.vert)
	FOREACH (FILENAME ${FRAGMENT_SHADERS})
		COMPILE_SHADER (FRAGMENT ${FILENAME} ${DIR_OUT})
	ENDFOREACH ()
	FOREACH (FILENAME ${VERTEX_SHADERS})
		COMPILE_SHADER (VERTEX ${FILENAME} ${DIR_OUT})
	ENDFOREACH ()
ENDMACRO ()

MACRO (COMPILE_TEXTURES_DIR DIR_IN DIR_OUT)
	FILE (GLOB_RECURSE TEXTURES ${DIR_IN}/*.*)
	FOREACH (FILENAME ${TEXTURES})
		COMPILE_TEXTURE (${FILENAME} ${DIR_OUT})
	ENDFOREACH ()
ENDMACRO ()

MACRO (COPY_RESOURCES_DIR DIR_IN DIR_OUT)
	FILE (GLOB_RECURSE RESOURCES ${DIR_IN}/*.*)
	FOREACH (FILENAME ${RESOURCES})
		FILE (RELATIVE_PATH FILEPATH ${DIR_IN} ${FILENAME})
		GET_FILENAME_COMPONENT (FILEPATH ${FILEPATH} DIRECTORY)
		FILE (COPY ${FILENAME} DESTINATION ${DIR_OUT}/${FILEPATH})
	ENDFOREACH ()
ENDMACRO ()
