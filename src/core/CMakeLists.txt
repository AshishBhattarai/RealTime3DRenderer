add_library(serializer-lib serializer.cpp) 

if(TEST_ENABLED)
    add_executable(serializer-test
        serializer_test_main.cpp
        serializer_test.cpp
    )
    target_link_libraries(serializer-test serializer-lib)
endif()

