list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Blobs)

add_blobs(blobs
    "shaders/gl3/main-fs.glsl"
    "shaders/gl3/main-vs.glsl"
    "shaders/gl3/ui-fs.glsl"
    "shaders/gl3/ui-vs.glsl"
)
