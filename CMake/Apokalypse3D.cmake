MACRO (COMPILE_SHADER TYPE IN DIR_OUT)
	SET (PLATFORMS_LIST glsl spirv essl dx9 dx11 msl)
	SET (ARGS_glsl --platform linux -p 120)
	SET (ARGS_spirv --platform linux -p spirv)
	SET (ARGS_essl --platform android)
	SET (ARGS_msl --platform osx -p metal)
	SET (ARGS_pssl --platform orbis -p pssl)
	IF (${TYPE} STREQUAL VERTEX)
		SET (PROFILE_dx9 --platform windows -O 3 -p vs_3_0)
		SET (PROFILE_dx11 --platform windows -O 3 -p vs_5_0)
		SET (SHADER_TYPE v)
	ELSEIF (${TYPE} STREQUAL FRAGMENT)
		SET (PROFILE_dx9 --platform windows -O 3 -p ps_3_0)
		SET (PROFILE_dx11 --platform windows -O 3 -p ps_5_0)
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
			VERBATIM
		)
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
