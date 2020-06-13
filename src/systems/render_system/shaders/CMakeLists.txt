 
add_subdirectory(glsl)

add_library(shaders-lib
    uniform_buffer.cpp
    program.cpp
    flat_forward_program.cpp
    skybox_shader.cpp
)
