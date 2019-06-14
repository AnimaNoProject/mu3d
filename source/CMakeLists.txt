# min cmake version
cmake_minimum_required(VERSION 3.10.2)

# project name
project(3d-mesh-unfolding)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Boost settings
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# external libraries
find_package(CGAL)
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(OpenGL)
find_package(Boost)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${OpenGL_INCLUDE_DIRS})

# all source files
set(3d-mesh-unfolding_SRCS
    main.cpp
    mainwindow.h
    mainwindow.cpp
    oglwidget.h
    oglwidget.cpp
    oglplanarwidget.h
    oglplanarwidget.cpp
    utility.h
    utility.cpp
    model.h
    model.cpp
    camera.h
    camera.cpp
    graph.h
    graph.cpp
    edge.h
    edge.cpp
    gluetag.h
    gluetag.cpp
    facetoplane.h
    facetoplane.cpp
    gluetagtoplane.h
    gluetagtoplane.cpp
)

# create the executeable from all source files
add_executable(${PROJECT_NAME} ${3d-mesh-unfolding_SRCS})
target_link_libraries(${PROJECT_NAME} CGAL::CGAL Qt5::Widgets ${OPENGL_gl_LIBRARY} ${Boost_LIBRARIES})
