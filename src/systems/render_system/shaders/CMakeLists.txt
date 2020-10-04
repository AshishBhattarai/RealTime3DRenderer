 
add_subdirectory(glsl)

add_library(shaders-lib
    uniform_buffer.cpp
    general_vs_ubo.cpp
    program.cpp
    flat_forward_material.cpp
    texture_forward_material.cpp
    skybox_shader.cpp
    visual_prep.cpp
    ibl_specular_convolution.cpp
    cubemap.cpp
    gui_shader.cpp
    grid_plane.cpp
)
