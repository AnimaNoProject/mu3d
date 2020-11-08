cmake_minimum_required (VERSION 3.8)

include_directories("../glm/glm")
add_executable (test main.cpp)

target_link_libraries(test LINK_PUBLIC MU3D)