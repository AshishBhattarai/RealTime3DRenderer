add_library(ecs-lib
    coordinator.cpp
    entity_manager.cpp
    component_manager.cpp
    system_manager.cpp
    event_manager.cpp)
target_include_directories(ecs-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
