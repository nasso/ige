cmake_minimum_required(VERSION 3.11)

project(ige)

if(${PROJECT_NAME} STREQUAL ${CMAKE_PROJECT_NAME})
	option(PACKAGE_TESTS "Build the tests" ON)
else()
	option(PACKAGE_TESTS "Build the tests" OFF)
endif()

add_subdirectory(ige)
add_subdirectory(examples)

# tests
if(PACKAGE_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
