list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(Blobs)

add_blobs(blobs
    "shaders/gl/gbuffer-fs.glsl"
    "shaders/gl/gbuffer-skin-vs.glsl"
    "shaders/gl/gbuffer-vs.glsl"
    "shaders/gl/light-pass-fs.glsl"
    "shaders/gl/light-pass-vs.glsl"
    "shaders/gl/ui-img-fs.glsl"
    "shaders/gl/ui-img-vs.glsl"
    "shaders/gl/ui-rect-fs.glsl"
    "shaders/gl/ui-rect-vs.glsl"
)
