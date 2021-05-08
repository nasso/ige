include(FetchContent)

# GLAD - OpenGL & Vulkan loader
add_subdirectory(glad)

# GLFW - Windowing library
FetchContent_Declare(glfw
    URL https://github.com/glfw/glfw/releases/download/3.3.4/glfw-3.3.4.zip
    URL_HASH SHA1=29ce5f24fede4935b4872606df1e987400c9d060
)
FetchContent_GetProperties(glfw)
if(NOT glfw_POPULATED)
    FetchContent_Populate(glfw)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR})
endif()

# source files
set(sources ${sources}
    "include/ige.hpp"
    "include/ige/core.hpp"
    "include/ige/core/Any.hpp"
    "include/ige/core/App.hpp"
    "include/ige/core/EventChannel.hpp"
    "include/ige/core/State.hpp"
    "include/ige/core/StateMachine.hpp"
    "include/ige/ecs.hpp"
    "include/ige/ecs/Component.hpp"
    "include/ige/ecs/MapStorage.hpp"
    "include/ige/ecs/Resources.hpp"
    "include/ige/ecs/Schedule.hpp"
    "include/ige/ecs/System.hpp"
    "include/ige/ecs/VecStorage.hpp"
    "include/ige/ecs/World.hpp"
    "include/ige/game.hpp"
    "include/ige/game/PerspectiveCamera.hpp"
    "include/ige/game/RenderingPlugin.hpp"
    "include/ige/game/Transform.hpp"
    "include/ige/game/WindowingPlugin.hpp"
    "include/ige/math.hpp"
    "include/ige/math/Vector.hpp"

    "src/core/Any.cpp"
    "src/core/App.cpp"
    "src/core/State.cpp"
    "src/core/StateMachine.cpp"
    "src/ecs/Schedule.cpp"
    "src/ecs/System.cpp"
    "src/ecs/World.cpp"
    "src/game/RenderingPlugin.cpp"
    "src/game/WindowingPlugin.cpp"
)

# static library
add_library(ige STATIC ${sources})
target_include_directories(ige
    PRIVATE "include"
    SYSTEM INTERFACE "include"
)
set_target_properties(ige
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        COMPILE_DEFINITIONS IGE_OPENGL
)
target_compile_options(ige
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
            -Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>
)
target_link_libraries(ige glfw glad)
