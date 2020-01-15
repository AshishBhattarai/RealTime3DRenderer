cmake_minimum_required(VERSION 3.5)

project(RealTime3DRenderer LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# directories
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE PATH "")
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_definitions( -DDISABLE_TESTS )

add_subdirectory(src)
