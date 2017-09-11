cmake_minimum_required (VERSION 2.8)
project(paths_photonics)

# ================ compiler
set(CMAKE_CXX_FLAGS "-std=c++14 -O3")

# ================ include dirs
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/Astar_justinhj)

# ================ sub dirs
add_subdirectory(src)
add_subdirectory(Tests)
