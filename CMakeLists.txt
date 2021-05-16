get_filename_component(PROJECT_NAME ${CMAKE_CURRENT_SOURCE_DIR} NAME)

project(${PROJECT_NAME})
cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

include_directories(${PROJECT_SOURCE_DIR}/external/eigen)
include_directories(${PROJECT_SOURCE_DIR}/external/voronoi/src)
include_directories(${PROJECT_SOURCE_DIR}/external/stb)


add_executable(lowpoly_cvt
        src/lowpoly_cvt.cpp
        src/JCVoronoi.cpp
        src/IOUtils.cpp src/IOUtils.h)
configure_file(python/run.py ${CMAKE_BINARY_DIR}/run.py COPYONLY)