
add_subdirectory(shaders)

add_library(render-system-lib
    render_system.cpp
    model.cpp
    render_defaults.cpp
    camera.cpp

    #non cpp files
    render_system_model.qmodel
    shaders
)
