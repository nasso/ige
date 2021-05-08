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
    "include/ige/game/Transform.hpp"
    "include/ige/game/WindowSettings.hpp"
    "include/ige/math.hpp"
    "include/ige/math/Vector.hpp"

    "src/core/Any.cpp"
    "src/core/App.cpp"
    "src/core/State.cpp"
    "src/core/StateMachine.cpp"
    "src/ecs/Schedule.cpp"
    "src/ecs/System.cpp"
    "src/ecs/World.cpp"
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
)
target_compile_options(ige
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:
            -Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:
            /W4>
)
