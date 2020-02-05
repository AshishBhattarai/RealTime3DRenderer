set(SHADER_OUTPUT_DIR ${CMAKE_BINARY_DIR}/shaders)

function(add_spirv_shader INPUT_FILE OUTPUT_FILE)
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT_DIR}/${OUTPUT_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND glslangValidator -e main -o ${SHADER_OUTPUT_DIR}/${OUTPUT_FILE} -V -G ${INPUT_FILE}
        MAIN_DEPENDENCY ${INPUT_FILE}
        COMMENT "Compile glsl shaders to spv"
    )
endfunction()

add_spirv_shader(flat_forward.vert flat_forward_vert.spv)
add_spirv_shader(flat_forward.frag flat_forward_frag.spv)

add_custom_target(glsl-code ALL DEPENDS
    ${SHADER_OUTPUT_DIR}/flat_forward_vert.spv
    ${SHADER_OUTPUT_DIR}/flat_forward_frag.spv
)
