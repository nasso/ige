cmake_minimum_required(VERSION 3.8)

project(ige_examples)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(demo "demo.cpp")
target_link_libraries(demo ige)
