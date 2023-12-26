cmake_minimum_required(VERSION 3.21)
project(oc_lab1)

set(CMAKE_CXX_STANDARD 23)

add_executable(oc_lab1 src/main.cpp src/child1.cpp src/child2.cpp src/utils.cpp include/utils.h)
