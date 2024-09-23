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
file(GLOB SHADER_SRC_FILES ${PROJECT_SOURCE_DIR}/shaders/*.frag ${PROJECT_SOURCE_DIR}/shaders/*.vert)

find_package(Vulkan REQUIRED OPTIONAL_COMPONENTS glslc)

add_compile_definitions(PROJ_DIR="${PROJECT_SOURCE_DIR}")

add_executable(Main "examples/Main.cpp" ${SRC_FILES})

target_link_libraries(Main Vulkan::Vulkan glfw)

if (Vulkan_glslc_FOUND)
	message("glslc found")

	set(SHADER_COMMANDS)
	set(SHADER_PRODUCTS)

	foreach(SHADER_SOURCE IN LISTS SHADER_SRC_FILES)
		cmake_path(GET SHADER_SOURCE FILENAME SHADER_NAME)

		# Build command
		list(APPEND SHADER_COMMANDS COMMAND)
		list(APPEND SHADER_COMMANDS Vulkan::glslc)
		list(APPEND SHADER_COMMANDS "${SHADER_SOURCE}")
		list(APPEND SHADER_COMMANDS "-o")
		list(APPEND SHADER_COMMANDS "${PROJECT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv")

		# Add product
		list(APPEND SHADER_PRODUCTS "${PROJECT_SOURCE_DIR}/shaders/${SHADER_NAME}.spv")
	endforeach()

	add_custom_target(CompileShaders ALL
		${SHADER_COMMANDS}
		COMMENT "Compiling Shaders"
		SOURCES ${SHADER_SRC_FILES}
		BYPRODUCTS ${SHADER_PRODUCTS})

	add_dependencies(Main CompileShaders)
else()
	message("glslc not found")
endif()