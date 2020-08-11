file(MAKE_DIRECTORY ${EXECUTABLE_OUTPUT_PATH}/shaders)
set(SHADER_OUTPUT_DIR ${EXECUTABLE_OUTPUT_PATH}/shaders)

function(add_spirv_shader INPUT_FILE OUTPUT_FILE BUILD_OPTIONS)
    add_custom_command(
        OUTPUT ${SHADER_OUTPUT_DIR}/${OUTPUT_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMAND glslangValidator -e main -o ${SHADER_OUTPUT_DIR}/${OUTPUT_FILE} -V -G ${INPUT_FILE} ${BUILD_OPTIONS}
        MAIN_DEPENDENCY ${INPUT_FILE}
        COMMENT "Compile glsl shaders to spv"
    )
endfunction()

add_spirv_shader(forward_material.vert flat_forward_material_vert.spv "")
add_spirv_shader(forward_material.frag flat_forward_material_frag.spv "")
add_spirv_shader(forward_material.vert texture_forward_material_vert.spv "-DTEXTURE_MATERIAL")
add_spirv_shader(forward_material.frag texture_forward_material_frag.spv "-DTEXTURE_MATERIAL")
add_spirv_shader(skybox.vert skybox_vert.spv "")
add_spirv_shader(cubemap.vert cubemap_vert.spv "")
add_spirv_shader(cubemap.frag cubemap_frag.spv "")
add_spirv_shader(cubemap.frag equirectangular_frag.spv "-DEQUIRECTANGULAR")
add_spirv_shader(visualprep.vert visualprep_vert.spv "")
add_spirv_shader(visualprep.frag visualprep_frag.spv "")
add_spirv_shader(ibl_diffuse_convolution.frag ibl_diffuse_convolution_frag.spv "")
add_spirv_shader(ibl_specular_convolution.frag ibl_specular_convolution_frag.spv "")
add_spirv_shader(ibl_brdf_integration_map.frag ibl_brdf_integration_map.spv "")

###
# Command created by add_custom_command() is not being run when you type make.
# You need to add_custom_target() with ALL keyword and make it depend on file,
# which is produced by command
###
add_custom_target(glsl-code ALL DEPENDS
    ${SHADER_OUTPUT_DIR}/flat_forward_material_vert.spv
    ${SHADER_OUTPUT_DIR}/flat_forward_material_frag.spv
    ${SHADER_OUTPUT_DIR}/texture_forward_material_vert.spv
    ${SHADER_OUTPUT_DIR}/texture_forward_material_frag.spv
    ${SHADER_OUTPUT_DIR}/skybox_vert.spv
    ${SHADER_OUTPUT_DIR}/cubemap_vert.spv
    ${SHADER_OUTPUT_DIR}/cubemap_frag.spv
    ${SHADER_OUTPUT_DIR}/equirectangular_frag.spv
    ${SHADER_OUTPUT_DIR}/visualprep_vert.spv
    ${SHADER_OUTPUT_DIR}/visualprep_frag.spv
    ${SHADER_OUTPUT_DIR}/ibl_diffuse_convolution_frag.spv
    ${SHADER_OUTPUT_DIR}/ibl_specular_convolution_frag.spv
    ${SHADER_OUTPUT_DIR}/ibl_brdf_integration_map.spv
)
