set(glad_SOURCES
    "include/glad/gl.h"
    "include/glad/vulkan.h"
    "include/KHR/khrplatform.h"
    "include/vk_platform.h"
    "src/gl.c"
    "src/vulkan.c"
)

add_library(glad ${glad_SOURCES})
target_include_directories(glad PUBLIC include)
