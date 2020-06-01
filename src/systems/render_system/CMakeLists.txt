
add_subdirectory(shaders)

add_library(render-system-lib
    render_system.cpp
    scene.cpp
    render_defaults.cpp
    camera.cpp
    renderer.cpp
    texture.cpp
    frame_buffer.cpp

    #non cpp files
    render_system_model.qmodel
)

target_link_libraries(render-system-lib shaders-lib)
