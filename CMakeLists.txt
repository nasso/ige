cmake_minimum_required(VERSION 3.8)

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
	include(GoogleTest)
	set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
	if (NOT TARGET gtest)
		add_subdirectory(extern/googletest)
	endif()
	set_target_properties(gtest PROPERTIES FOLDER extern)
	set_target_properties(gtest_main PROPERTIES FOLDER extern)
	set_target_properties(gmock PROPERTIES FOLDER extern)
	set_target_properties(gmock_main PROPERTIES FOLDER extern)

	add_subdirectory(tests)
endif()
