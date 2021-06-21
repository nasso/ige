add_requires("glm >=0.9.9", "bullet3 >=3.09", "openal-soft >=1.21.1")

target("glad")
    set_kind("static")
    add_files("glad/src/**.c")
    add_includedirs("glad/include", {public=true})

target("stb")
    set_kind("static")
    add_includedirs("stb", {public=true})

target("ige")
    set_kind("static")
    set_languages("cxx20")
    add_files("src/**.cpp")
    add_includedirs("include", {public=true})
    set_warnings("all")
    add_deps("glad", "stb")
    add_packages("glm", "bullet3", "openal-soft")
