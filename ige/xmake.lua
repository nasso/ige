add_requires(
    "glm ^0.9.9",
    "bullet3 ^3.09",
    "nlohmann_json ^3.9.1",
    "glfw ^3.3.4",
    "fx-gltf ^1.2.0",
    "libnyquist master"
)
add_requires("openal-soft ^1.21.1", {configs={shared=true}})

includes("blobs")
includes("glad")
includes("stb")

target("ige")
    set_kind("static")

    -- enable C++20
    set_languages("cxx20")

    -- enable all warnings
    set_warnings("all")

    -- source + includes
    add_files("src/**.cpp")
    add_includedirs("include", {public=true})

    -- define some symbols
    add_defines(
        "_CRT_SECURE_NO_WARNINGS",
        "IGE_OPENGL"
    )

    -- dependencies
    add_deps("blobs", "glad", "stb")
    add_packages(
        "glm",
        "bullet3",
        "glfw",
        "openal-soft",
        "nlohmann_json",
        "fx-gltf",
        "libnyquist",
        {public=true}
    )
