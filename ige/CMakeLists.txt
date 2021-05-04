# source files
set(sources ${sources}
    "include/ige.hpp"
    "include/ige/core.hpp"
    "include/ige/core/Any.hpp"
    "include/ige/core/App.hpp"
    "include/ige/core/State.hpp"
    "include/ige/core/StateMachine.hpp"
    "include/ige/ecs.hpp"
    "include/ige/ecs/MapStorage.hpp"
    "include/ige/ecs/Schedule.hpp"
    "include/ige/ecs/System.hpp"
    "include/ige/ecs/VecStorage.hpp"
    "include/ige/ecs/World.hpp"
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
target_include_directories(ige PUBLIC "include")
set_target_properties(ige
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
