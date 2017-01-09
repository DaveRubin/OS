cmake_minimum_required(VERSION 3.6)
project(q1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES step3/triangles.c)
add_executable(q1 ${SOURCE_FILES})