cmake_minimum_required(VERSION 3.8)

# we use C++ 14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# source files
set(sources ${sources}
    "include/un.hpp"
    "include/un/App.hpp"
    "include/un/ecs.hpp"
    "include/un/core.hpp"
    "include/un/core/Any.hpp"
    "include/un/ecs/MapStorage.hpp"
    "include/un/ecs/VecStorage.hpp"
    "include/un/ecs/System.hpp"
    "include/un/ecs/World.hpp"
    "src/App.cpp"
    "src/core/Any.cpp"
    "src/ecs/System.cpp"
    "src/ecs/World.cpp"
)

# static library
add_library(un STATIC ${sources})
target_link_libraries(un rtl)
target_include_directories(un PUBLIC "include")
