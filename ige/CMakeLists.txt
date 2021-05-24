include(FetchContent)

# The blobs subdirectory contains files embedded statically at compile time
add_subdirectory(blobs)

# STB - Single-file Public Domain libraries for C/C++
add_subdirectory(stb)

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

# GLM - OpenGL Mathematics
FetchContent_Declare(glm
    URL https://github.com/g-truc/glm/releases/download/0.9.9.8/glm-0.9.9.8.zip
    URL_HASH SHA1=45408897f419944fb28d8fd835791f237be2ec19
)
FetchContent_MakeAvailable(glm)

# nlohmann/json - JSON library
FetchContent_Declare(json
    GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
    GIT_TAG v3.9.1
)
FetchContent_MakeAvailable(json)

# fx-gltf - glTF loader
FetchContent_Declare(fx-gltf
    URL https://github.com/nasso/fx-gltf/archive/refs/heads/ige.zip
)
FetchContent_GetProperties(fx-gltf)
if(NOT fx-gltf_POPULATED)
    FetchContent_Populate(fx-gltf)
    set(FX_GLTF_INSTALL OFF CACHE BOOL "" FORCE)
    set(FX_GLTF_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${fx-gltf_SOURCE_DIR} ${fx-gltf_BINARY_DIR})
endif()

# Bullet3 - Physics engine
FetchContent_Declare(bullet3
    GIT_REPOSITORY https://github.com/bulletphysics/bullet3.git
    GIT_TAG 3.17
)
FetchContent_GetProperties(bullet3)
if(NOT bullet3_POPULATED)
    FetchContent_Populate(bullet3)
    set(OpenGL_GL_PREFERENCE "GLVND" CACHE STRING "" FORCE)
    set(BUILD_BULLET2_DEMOS OFF CACHE BOOL "" FORCE)
    set(BUILD_EXTRAS OFF CACHE BOOL "" FORCE)
    set(BUILD_UNIT_TESTS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${bullet3_SOURCE_DIR} ${bullet3_BINARY_DIR})
endif()

# Libnyquist - Audio File Parsing
FetchContent_Declare(libnyquist
    URL https://github.com/ddiakopoulos/libnyquist/archive/refs/heads/master.zip)
FetchContent_MakeAvailable(libnyquist)

# source files
set(sources ${sources}
    "include/ige.hpp"
    "include/ige/asset.hpp"
    "include/ige/asset/Material.hpp"
    "include/ige/asset/Mesh.hpp"
    "include/ige/asset/Texture.hpp"
    "include/ige/core.hpp"
    "include/ige/core/Any.hpp"
    "include/ige/core/App.hpp"
    "include/ige/core/EventChannel.hpp"
    "include/ige/core/State.hpp"
    "include/ige/core/StateMachine.hpp"
    "include/ige/core/TypeId.hpp"
    "include/ige/ecs.hpp"
    "include/ige/ecs/Component.hpp"
    "include/ige/ecs/Entity.hpp"
    "include/ige/ecs/MapStorage.hpp"
    "include/ige/ecs/Resources.hpp"
    "include/ige/ecs/Schedule.hpp"
    "include/ige/ecs/System.hpp"
    "include/ige/ecs/VecStorage.hpp"
    "include/ige/ecs/World.hpp"
    "include/ige/plugin.hpp"
    "include/ige/plugin/GltfPlugin.hpp"
    "include/ige/plugin/input/Controller.hpp"
    "include/ige/plugin/input/InputManager.hpp"
    "include/ige/plugin/input/InputRegistry.hpp"
    "include/ige/plugin/input/Keyboard.hpp"
    "include/ige/plugin/input/Mouse.hpp"
    "include/ige/plugin/InputPlugin.hpp"
    "include/ige/plugin/InputPlugin.hpp"
    "include/ige/plugin/physics/GhostObject.hpp"
    "include/ige/plugin/physics/PhysicsWorld.hpp"
    "include/ige/plugin/physics/RigidBody.hpp"
    "include/ige/plugin/PhysicsPlugin.hpp"
    "include/ige/plugin/RenderPlugin.hpp"
    "include/ige/plugin/ScriptPlugin.hpp"
    "include/ige/plugin/TimePlugin.hpp"
    "include/ige/plugin/TransformPlugin.hpp"
    "include/ige/plugin/WindowPlugin.hpp"
    "include/ige/plugin/AudioPlugin/AudioEngine.hpp"
    "include/ige/plugin/AudioPlugin/AudioPlugin.hpp"
    "include/ige/plugin/AudioPlugin/AudioClip.hpp"
    "include/ige/plugin/AudioPlugin/AudioSource.hpp"
    "include/ige/plugin/AudioPlugin/exceptions/AudioPluginException.hpp"

    "src/asset/Material.cpp"
    "src/asset/Mesh.cpp"
    "src/asset/Texture.cpp"
    "src/core/Any.cpp"
    "src/core/App.cpp"
    "src/core/State.cpp"
    "src/core/StateMachine.cpp"
    "src/ecs/Entity.cpp"
    "src/ecs/Schedule.cpp"
    "src/ecs/System.cpp"
    "src/ecs/World.cpp"
    "src/plugin/GltfPlugin.cpp"
    "src/plugin/input/InputManager.cpp"
    "src/plugin/input/Keyboard.cpp"
    "src/plugin/input/Mouse.cpp"
    "src/plugin/InputPlugin.cpp"
    "src/plugin/InputPlugin.cpp"
    "src/plugin/physics/bullet3/BulletGhostObject.cpp"
    "src/plugin/physics/bullet3/BulletGhostObject.hpp"
    "src/plugin/physics/bullet3/BulletRigidBody.cpp"
    "src/plugin/physics/bullet3/BulletRigidBody.hpp"
    "src/plugin/physics/bullet3/BulletWorld.cpp"
    "src/plugin/physics/bullet3/BulletWorld.hpp"
    "src/plugin/physics/GhostObject.cpp"
    "src/plugin/physics/PhysicsWorld.cpp"
    "src/plugin/physics/RigidBody.cpp"
    "src/plugin/PhysicsPlugin.cpp"
    "src/plugin/renderer/backend.hpp"
    "src/plugin/renderer/opengl.cpp"
    "src/plugin/renderer/opengl/Buffer.cpp"
    "src/plugin/renderer/opengl/Buffer.hpp"
    "src/plugin/renderer/opengl/Error.cpp"
    "src/plugin/renderer/opengl/Error.hpp"
    "src/plugin/renderer/opengl/Program.cpp"
    "src/plugin/renderer/opengl/Program.hpp"
    "src/plugin/renderer/opengl/Shader.cpp"
    "src/plugin/renderer/opengl/Shader.hpp"
    "src/plugin/renderer/opengl/Texture.cpp"
    "src/plugin/renderer/opengl/Texture.hpp"
    "src/plugin/renderer/opengl/VertexArray.cpp"
    "src/plugin/renderer/opengl/VertexArray.hpp"
    "src/plugin/RenderPlugin.cpp"
    "src/plugin/ScriptPlugin.cpp"
    "src/plugin/TimePlugin.cpp"
    "src/plugin/TransformPlugin.cpp"
    "src/plugin/WindowPlugin.cpp"
)

# compile options
set(compile_flags
    _CRT_SECURE_NO_WARNINGS
    GLM_FORCE_SILENT_WARNINGS
    IGE_OPENGL
)

# static library
add_library(ige STATIC ${sources})
target_include_directories(ige
    PRIVATE "include"
    SYSTEM INTERFACE "include"
    PRIVATE ${BULLET_PHYSICS_SOURCE_DIR}/src
)
set_target_properties(ige
    PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
)
target_compile_options(ige
    PRIVATE
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127>
)
target_compile_definitions(ige PUBLIC ${compile_flags})
target_link_libraries(ige
    blobs
    stb
    glfw
    glad
    glm
    nlohmann_json::nlohmann_json
    fx-gltf::fx-gltf
    BulletDynamics
    BulletCollision
    LinearMath
    OpenAL
    libnyquist
)

