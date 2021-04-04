cmake_minimum_required(VERSION 3.11)

project(un)

if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})
	option(PACKAGE_TESTS "Build the tests" ON)
else()
	option(PACKAGE_TESTS "Build the tests" OFF)
endif()

add_subdirectory(extern/rtl)
add_subdirectory(un)
add_subdirectory(examples)

# tests
if(PACKAGE_TESTS)
	enable_testing()

	add_subdirectory(tests)
endif()
