include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)


set(GLAD_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad)
set(GLFW_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw)
set(GLM_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)
set(ASIO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party/asio/asio)

# opengl
if(POLICY CMP0072)
  cmake_policy(SET CMP0072 NEW)
endif()
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})

# glad
set(GLAD_LIBRARIES glad)
include_directories(${GLAD_DIR}/include)

#glfw
if(USE_SHARED_GLFW)
    find_package(glfw3 3.3 REQUIRED)
else()
    set(GLFW_INCLUDE_DIR ${GLFW_DIR}/include)
endif()
set(GLFW_LIBRARIES glfw)
include_directories(${GLFW_INCLUDE_DIR})

#glm
if(USE_SHARED_GLM)
    find_package(glm REQUIRED)
else()
    set(GLM_INCLUDE_DIR ${GLM_DIR})
endif()
include_directories(${GLM_INCLUDE_DIR})

#asio
if(USE_SHARED_ASIO)
    find_path(ASIO_INCLUDE_DIR asio.hpp HINTS 
        "/usr/include"
        "/usr/local/include"
        "/opt/local/include"
    )
else()
    set(ASIO_INCLUDE_DIR ${ASIO_DIR}/include)
endif()
include_directories(${ASIO_INCLUDE_DIR})
    

# modules
add_subdirectory(third_party)
add_subdirectory(core)
add_subdirectory(ecs)
add_subdirectory(systems/render_system)
add_subdirectory(systems/world_system)
add_subdirectory(app)

# exec
add_executable(renderer-demo main.cpp)
target_link_libraries(renderer-demo app-lib serializer-lib)
