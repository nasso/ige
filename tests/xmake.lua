add_requires("gtest ^1.11.0", {configs={main=true, gmock=true}})

target("tests")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_deps("ige")
    add_packages("gtest")

    if is_plat("windows") then
        add_ldflags("/subsystem:console")
    end

    add_files("**.cpp")
