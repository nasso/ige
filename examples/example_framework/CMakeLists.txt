add_library(example_framework
    TrackballCamera.hpp
    TrackballCamera.cpp
)
target_include_directories(example_framework PUBLIC .)
target_link_libraries(example_framework ige)
set_target_properties(example_framework
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
