add_library(common
    TrackballCamera.hpp
    TrackballCamera.cpp
)
target_include_directories(common PUBLIC .)
target_link_libraries(common ige)
set_target_properties(common
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
