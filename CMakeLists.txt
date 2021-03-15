cmake_minimum_required(VERSION 3.8)

project(un)
include(CTest)

# modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(rtl)
add_subdirectory(un)
add_subdirectory(examples)

# tests
if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
