target("ige")
    set_kind("shared")
    set_languages("cxx20")
    set_warnings("all")

    -- public header files
    add_includedirs("include", {public=true})
    add_headerfiles("include/(**.hpp)")
    add_headerfiles("include/(**.inl)")

    -- private header files
    add_includedirs("common/include")
    set_pcxxheader("common/include/igepch.hpp")

    -- source files
    add_files("src/**.cpp")

    -- define some symbols
    add_defines("IGE_BUILD_DLL")

    -- define IGE_DEBUG when in debug mode
    if is_mode("debug") then
        add_defines("IGE_DEBUG")
    end

    if is_plat("windows") then
        add_defines("_CRT_SECURE_NO_WARNINGS")
        add_cxxflags("/Zc:__cplusplus")
    end
