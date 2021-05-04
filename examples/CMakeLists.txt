cmake_minimum_required(VERSION 3.8)

project(ige_examples)

add_executable(demo "demo.cpp")
target_link_libraries(demo ige)
set_target_properties(demo
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
