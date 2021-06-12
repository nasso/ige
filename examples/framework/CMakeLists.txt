add_library(framework
    TrackballCamera.hpp
    TrackballCamera.cpp
)
target_include_directories(framework PUBLIC .)
target_link_libraries(framework ige)
set_target_properties(framework
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
