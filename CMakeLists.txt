cmake_minimum_required(VERSION 3.5)

project(RealTime3DRenderer LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions")

# directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#options
option(TEST_ENABLED "build tests" on)
option(BUILD_SHARED_LIBS "build the shared libs" off)
if(BUILD_SHARED_LIBS)
    option(USE_SHARED_GLFW "Use the shared glfw library" off)
    option(USE_SHARED_GLM "Use the shared glm library" off)
else()
    option(USE_SHARED_GLFW "Use the shared glfw library" on)
    option(USE_SHARED_GLM "Use the shared glm library" on)
endif()

add_subdirectory(src)
