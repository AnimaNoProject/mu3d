cmake_minimum_required (VERSION 3.8)

project(MU3D CXX)

find_package(CGAL)

include_directories("../glm/glm")
include_directories("include")

add_library(MU3D STATIC   
						"include/graph.h" 
						"include/edge.h" 
						"include/gluetag.h" 
						"include/facetoplane.h"  
						"include/gluetagtoplane.h" 
						"include/utility.h"
						 
						"graph.cpp" 
						"edge.cpp" 
						"gluetag.cpp"
						"facetoplane.cpp"
						"gluetagtoplane.cpp"
						"utility.cpp")

target_link_libraries(MU3D CGAL::CGAL)
target_include_directories(MU3D PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})