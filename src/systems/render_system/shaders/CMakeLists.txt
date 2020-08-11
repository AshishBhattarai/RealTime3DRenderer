 
add_subdirectory(glsl)

add_library(shaders-lib
    uniform_buffer.cpp
    program.cpp
    flat_forward_material.cpp
    flat_forward_material.cpp
    skybox_shader.cpp
    visual_prep.cpp
    ibl_specular_convolution.cpp
    cubemap.cpp
)
