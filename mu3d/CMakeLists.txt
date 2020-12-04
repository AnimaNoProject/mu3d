cmake_minimum_required (VERSION 3.8)

project(mu3d CXX)

find_package(CGAL)

include_directories("../glm/glm")
include_directories("include")

add_library(mu3d STATIC   
						"include/graph.h" 
						"include/edge.h" 
						"include/gluetab.h" 
						"include/facetoplane.h"  
						"include/gluetabtoplane.h" 
						"include/utility.h"
						 
						"graph.cpp" 
						"edge.cpp" 
						"gluetab.cpp"
						"facetoplane.cpp"
						"gluetabtoplane.cpp"
						"utility.cpp")

target_link_libraries(mu3d CGAL::CGAL)
target_include_directories(mu3d PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})