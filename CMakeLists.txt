cmake_minimum_required(VERSION 3.26)
project(Starlight)

set(CMAKE_CXX_STANDARD 20)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

include_directories(external/glm)
include_directories(include)

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)

find_package(Vulkan REQUIRED)

add_executable(Main "examples/Main.cpp" ${SRC_FILES})

target_link_libraries(Main Vulkan::Vulkan glfw)