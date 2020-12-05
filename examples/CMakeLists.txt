cmake_minimum_required (VERSION 3.8)

project(mu3d_example)

include_directories("../glm/glm")
include_directories("../mu3d")
add_executable (${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} LINK_PUBLIC mu3d)

add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_SOURCE_DIR}/examples/assets"
                "${CMAKE_CURRENT_BINARY_DIR}/assets")