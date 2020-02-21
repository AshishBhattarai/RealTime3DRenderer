
add_library(world-system-lib
    world_system.cpp
    world_object.cpp

    #non cpp files
    world_system_model.qmodel
)

if(TEST_ENABLED)
    add_executable(world-system-test system_test.cpp)
    target_link_libraries(world-system-test  world-system-lib ecs-lib)
endif()
