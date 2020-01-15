add_library(app-lib app.cpp)
target_link_libraries(app-lib ecs-lib)
target_include_directories(app-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
