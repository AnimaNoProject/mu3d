cmake_minimum_required (VERSION 3.8)

project(mu3d_test)

include_directories("../glm/glm")
add_executable (mu3d_test main.cpp)

target_link_libraries(mu3d_test LINK_PUBLIC MU3D)

add_custom_command(
        TARGET mu3d_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                "${CMAKE_SOURCE_DIR}/assets/octa.off"
                "${CMAKE_CURRENT_BINARY_DIR}/assets/octa.off")