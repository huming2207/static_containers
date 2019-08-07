cmake_minimum_required(VERSION 3.14)
project(static_containers)

set(CMAKE_CXX_STANDARD 17)

add_executable(static_containers test.cpp static_vector.hpp aligned_array.hpp)