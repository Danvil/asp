cmake_minimum_required(VERSION 2.6)

set(EIGEN3_INCLUDE_DIR "/usr/include/eigen3" CACHE STRING "eigen3 include directory")
set(SLIMAGE_INCLUDE_DIR "/home/david/git/slimage/include" CACHE STRING "slimage include directory")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/")

add_definitions(-std=c++11 -DBOOST_DISABLE_ASSERTS)

include_directories(
	${EIGEN3_INCLUDE_DIR}
	${SLIMAGE_INCLUDE_DIR}
	include
)

project(asp)

add_subdirectory(src/libasp)
add_subdirectory(src/asp)
