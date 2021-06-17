cmake_minimum_required(VERSION 3.17)

project(ige_examples)

add_subdirectory(framework)

macro(package_add_example EXAMPLE_NAME)
    add_executable(${EXAMPLE_NAME} ${ARGN})
    target_link_libraries(${EXAMPLE_NAME} ige framework)
    set_target_properties(${EXAMPLE_NAME}
        PROPERTIES
            CXX_STANDARD 20
            CXX_STANDARD_REQUIRED ON
            CXX_EXTENSIONS OFF
    )
    add_custom_command(
        TARGET ${EXAMPLE_NAME}
        POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:OpenAL>
            $<TARGET_FILE_DIR:${EXAMPLE_NAME}>
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
package_add_example(inputs inputs.cpp)
package_add_example(gamepad gamepad.cpp)
package_add_example(bindings bindings.cpp)
package_add_example_data(bindings config)
package_add_example(mouse mouse.cpp)
package_add_example(audio audio.cpp)
package_add_example_data(audio assets)
package_add_example(audio_controls audio_controls.cpp)
package_add_example_data(audio_controls assets)
package_add_example(hierarchy hierarchy.cpp)
package_add_example(gltf gltf.cpp)
package_add_example_data(gltf assets)
package_add_example(script script.cpp)
package_add_example(physics physics.cpp)
package_add_example_data(physics assets)
package_add_example(ui ui.cpp)
package_add_example_data(ui assets)
package_add_example(animation animation.cpp)
package_add_example_data(animation assets)
