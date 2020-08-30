add_library(
    app-lib
    app.cpp
    display.cpp
    input.cpp
    gui.cpp
    loaders.cpp
    app_config.cpp
    construct.cpp
    rtsp_client.cpp
    command_server.cpp
    app_model.qmodel
)
target_link_libraries(
    app-lib
    ecs-lib
    serializer-lib
    render-system-lib
    world-system-lib
    ${OPENGL_LIBRARIES}
    ${GLAD_LIBRARIES}
    ${GLFW_LIBRARIES}
    dl
    pthread
)
#target_include_directories(app-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

if(TEST_ENABLED)
    add_executable(
        app-test
        app_test_main.cpp
        command_test_client.cpp
    )
    target_link_libraries(app-test serializer-lib dl pthread)
endif()


