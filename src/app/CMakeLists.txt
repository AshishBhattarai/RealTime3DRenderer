add_library(
    app-lib
    app.cpp
    display.cpp
    app_model.qmodel
)
target_link_libraries(
    app-lib
    ecs-lib
    ${OPENGL_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${GLFW_LIBRARIES}
    dl
)
#target_include_directories(app-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
