cmake_minimum_required (VERSION 3.8)

project(mu3d CXX)

find_package(CGAL)

include_directories("../glm/glm")
include_directories("mu3d")

add_library(mu3d STATIC   
						"mu3d/graph.h" 
						"mu3d/edge.h" 
						"mu3d/gluetab.h" 
						"mu3d/facetoplane.h"  
						"mu3d/gluetabtoplane.h" 
						"mu3d/utility.hpp"
						 
						"graph.cpp" 
						"edge.cpp" 
						"gluetab.cpp"
						"facetoplane.cpp"
						"gluetabtoplane.cpp")

target_link_libraries(mu3d CGAL::CGAL)
target_include_directories(mu3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})