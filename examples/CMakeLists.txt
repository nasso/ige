cmake_minimum_required(VERSION 3.17)

project(ige_examples)

macro(package_add_example EXAMPLE_NAME)
    add_executable(${EXAMPLE_NAME} ${ARGN})
    target_link_libraries(${EXAMPLE_NAME} ige)
    set_target_properties(${EXAMPLE_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
endmacro()

macro(package_add_example_data EXAMPLE_NAME DATA_DIR)
    add_custom_command(
        TARGET ${EXAMPLE_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/${DATA_DIR}
            $<TARGET_FILE_DIR:${EXAMPLE_NAME}>/${DATA_DIR}
    )
endmacro()

package_add_example(cubes cubes.cpp)
package_add_example(hierarchy hierarchy.cpp)
package_add_example(gltf gltf.cpp)
package_add_example_data(gltf gltf)
