add_requires("gtest ^1.11.0", {configs={main=true, gmock=true}})

target("tests")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_deps("ige")
    add_packages("gtest")

    if is_plat("windows") then
        if get_config("toolchain") == "clang" then
            add_ldflags("-Wl,-subsystem:console")
        else
            add_defines("_CRT_SECURE_NO_WARNINGS")
            add_ldflags("/subsystem:console")
            add_cxxflags("/Zc:__cplusplus")
        end
    end

    add_files("**.cpp")