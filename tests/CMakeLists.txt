cmake_minimum_required(VERSION 3.8)

project(un_tests)

find_package(GTest)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(TARGET GTest::gtest_main AND TARGET GTest::gmock)
    # add test executable
    add_executable(un_unit_tests
        "any.cpp"
        "state.cpp"
        "statemachine.cpp"
        "storage.cpp"
        "world.cpp"
    )
    target_link_libraries(un_unit_tests un)
    target_link_libraries(un_unit_tests GTest::gmock)
    target_link_libraries(un_unit_tests GTest::gtest_main)

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
endif()
