cmake_minimum_required(VERSION 3.8)

find_package(Criterion REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(unit_tests
    "test.cpp"
)
target_link_libraries(unit_tests oge)
target_link_libraries(unit_tests ${CRITERION_LIBRARIES})
target_include_directories(unit_tests PRIVATE ${CRITERION_INCLUDE_DIRS})
