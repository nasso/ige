include(FetchContent)

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
    "include/ige/core/DataStore.hpp"
    "include/ige/core/EventChannel.hpp"
    "include/ige/core/FileSystemDataStore.hpp"
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
    "include/ige/plugin.hpp"
    "include/ige/plugin/RenderingPlugin.hpp"
    "include/ige/plugin/TransformPlugin.hpp"
    "include/ige/plugin/WindowingPlugin.hpp"

    "src/asset/Material.cpp"
    "src/asset/Mesh.cpp"
    "src/asset/Texture.cpp"
    "src/core/Any.cpp"
    "src/core/App.cpp"
    "src/core/DataStore.cpp"
    "src/core/FileSystemDataStore.cpp"
    "src/core/State.cpp"
    "src/core/StateMachine.cpp"
    "src/ecs/Schedule.cpp"
    "src/ecs/System.cpp"
    "src/ecs/World.cpp"
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
    "src/plugin/renderer/backend.hpp"
    "src/plugin/renderer/opengl.cpp"
    "src/plugin/RenderingPlugin.cpp"
    "src/plugin/TransformPlugin.cpp"
    "src/plugin/WindowingPlugin.cpp"
)

# compile options
get_filename_component(data_store_path data ABSOLUTE)
set(compile_flags
    GLM_FORCE_SILENT_WARNINGS
    IGE_OPENGL
    IGE_ENGINE_FS_DATA_STORE_PATH=${data_store_path}
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
        $<$<CXX_COMPILER_ID:Clang,AppleClang,GNU>:-Wall -Wextra>
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4103>
)
target_compile_definitions(ige PUBLIC ${compile_flags})
target_link_libraries(ige glfw glad glm)
