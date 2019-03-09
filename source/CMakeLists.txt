# min cmake version
cmake_minimum_required(VERSION 3.10.2)

# project name
project(3d-mesh-unfolding)

# include everything in the current directory
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# external libraries
find_package(CGAL)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(OpenGL)

# all source files
set(3d-mesh-unfolding_SRCS
    main.cpp
)

# create the executeable from all source files
add_executable(${PROJECT_NAME} ${3d-mesh-unfolding_SRCS})

target_link_libraries(${PROJECT_NAME} CGAL::CGAL Qt5::Widgets ${OPENGL_gl_LIBRARY})
