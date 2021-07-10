add_requires("gtest master", {configs={main=true, gmock=true}})

local tests = {
    ["any"] = { files = {"any.cpp"} },
    ["entity"] = { files = {"entity.cpp"} },
    ["eventchannel"] = { files = {"eventchannel.cpp"} },
    ["statemachine"] = { files = {"statemachine.cpp"} },
    ["storage"] = { files = {"storage.cpp"} },
    ["world"] = { files = {"world.cpp"} },
}

for name, cfg in pairs(tests) do
    target(name)
    set_group("tests")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_deps("ige")
    add_packages("gtest")

    if is_plat("windows") then
        add_ldflags("/subsystem:console")
    end

    for _, v in ipairs(cfg.files) do
        add_files(v)
    end
end
