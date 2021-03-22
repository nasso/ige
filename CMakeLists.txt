cmake_minimum_required(VERSION 3.8)

project(un)
include(CTest)

include(${CMAKE_BINARY_DIR}/conan_paths.cmake)

add_subdirectory(rtl)
add_subdirectory(un)
add_subdirectory(examples)

# tests
if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
