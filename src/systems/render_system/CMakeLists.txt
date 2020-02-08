
add_subdirectory(shaders)

add_library(render-system-lib
    render_system.cpp
    model.cpp
    render_defaults.cpp
    camera.cpp
    renderer.cpp
    texture.cpp

    #non cpp files
    render_system_model.qmodel
)

target_link_libraries(render-system-lib shaders-lib)
