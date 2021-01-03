cmake_minimum_required(VERSION 3.11)
project(Yedis)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g -Wall -O0 -Wno-unused-variable -pthread") 
add_subdirectory(base)
add_subdirectory(net)
add_subdirectory(src)
