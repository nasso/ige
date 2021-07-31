includes("framework")

local examples = {
    window = {
        files = {"window.cpp"};
    };
}

for name, cfg in pairs(examples) do
    target(name)

    set_group("examples")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_includedirs(".")
    add_deps("ige")
    set_rundir(os.scriptdir())

    for _, v in ipairs(cfg.files) do
        add_files(v)
    end
end
