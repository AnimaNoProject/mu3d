# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("3dmeshunfoldinglib")

# Include sub-projects.
add_subdirectory ("mu3d")
add_subdirectory ("examples")