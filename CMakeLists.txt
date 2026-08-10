cmake_minimum_required(VERSION 3.22.1)
project(freecam_mod CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,max-page-size=16384")

add_library(freecam SHARED main.cpp)
target_link_libraries(freecam log)
