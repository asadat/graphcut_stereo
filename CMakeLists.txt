cmake_minimum_required(VERSION 2.4.6 FATAL_ERROR)

SET(ProjName gstereo)

#include_directories("~/opencv-2.4.8/OpenCV-2.4.2/include")
include_directories("/usr/X11/include")
include_directories("./src/graphcut")
include_directories("~/libcvd")

#include_directories("/usr/include")
include_directories("/usr/local/include")
#include_directories("./src")

#link_directories("~/opencv-2.4.8/OpenCV-2.4.2/lib")
#link_directories("~/OpenCv-2.4.2/lib")
link_directories("/usr/X11/lib")

link_directories("~/libcvd")
#link_directories("/usr/lib")
link_directories("/usr/local/lib")

set(gstereo_LIBRARIES cvd gl)

#SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_CGAL_COMPILE_FLAGS}" )

add_executable(${ProjName}
src/graphcut/block.h
src/graphcut/graph.h
src/graphcut/graph.cpp
src/graphcut/instances.inc
src/graphcut/maxflow.cpp
src/main.cpp
src/GraphcutStereo.h
src/GraphcutStereo.cpp

)
target_link_libraries(${ProjName} ${gstereo_LIBRARIES})

