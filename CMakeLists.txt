cmake_minimum_required(VERSION 3.8)

project(oge)

# modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# options
option(ENABLE_TESTS "Build tests along with the library" ON)

# we use C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# source files
set(sources ${sources}
    "include/oge/App.hpp"
    "include/oge/ecs/System.hpp"
    "src/App.cpp"
    "src/ecs/System.cpp"
)

# static library
add_library(oge STATIC ${sources})
target_include_directories(oge PUBLIC "include")

# tests
if(ENABLE_TESTS)
	enable_testing()
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")

	install(TARGETS unit_tests COMPONENT tests EXCLUDE_FROM_ALL)
endif()
