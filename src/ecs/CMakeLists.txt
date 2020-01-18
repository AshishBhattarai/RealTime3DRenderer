add_library(ecs-lib
    coordinator.cpp
    entity_manager.cpp
    component_manager.cpp
    system_manager.cpp
    event_manager.cpp)

if(TEST_ENABLED)
    add_executable(ecs-test
        ecs_test_main.cpp
        entity_manager_test.cpp
        component_array_test.cpp
        component_manager_test.cpp
        system_manager_test.cpp
        event_manager_test.cpp
        coordinator_test.cpp
        )
    target_link_libraries(ecs-test ecs-lib)
endif()
