cmake_minimum_required(VERSION 3.8)

project(un_tests)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-src
    ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
    EXCLUDE_FROM_ALL
)

# add test executable
add_executable(un_unit_tests
    "any.cpp"
    "state.cpp"
    "statemachine.cpp"
    "storage.cpp"
    "world.cpp"
)
target_link_libraries(un_unit_tests un)
target_link_libraries(un_unit_tests gmock)
target_link_libraries(un_unit_tests gtest_main)

add_test(
    NAME un_unit_tests_build
    COMMAND "${CMAKE_COMMAND}"
            --build "${CMAKE_BINARY_DIR}"
            --config "$<CONFIG>"
            --target un_unit_tests
)
set_tests_properties(un_unit_tests_build
    PROPERTIES FIXTURES_SETUP un_unit_tests_fixture)

add_test(NAME un_unit_tests COMMAND un_unit_tests)
set_tests_properties(un_unit_tests
    PROPERTIES FIXTURES_REQUIRED un_unit_tests_fixture)
