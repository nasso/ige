includes("framework")

local examples = {
    ["cubes"] = { files = {"cubes.cpp"} },
    ["inputs"] = { files = {"inputs.cpp"} },
    ["gamepad"] = { files = {"gamepad.cpp"} },
    ["bindings"] = { files = {"bindings.cpp"} },
    ["mouse"] = { files = {"mouse.cpp"} },
    ["audio"] = { files = {"audio.cpp"} },
    ["audio_controls"] = { files = {"audio_controls.cpp"} },
    ["hierarchy"] = { files = {"hierarchy.cpp"} },
    ["gltf"] = { files = {"gltf.cpp"} },
    ["script"] = { files = {"script.cpp"} },
    ["physics"] = { files = {"physics.cpp"} },
    ["ui"] = { files = {"ui.cpp"} },
    ["animation"] = { files = {"animation.cpp"} },
}

for name, cfg in pairs(examples) do
    target(name)
    set_group("examples")
    set_kind("binary")
    set_languages("cxx20")
    set_warnings("all")
    add_deps("ige", "framework")
    set_rundir(os.scriptdir())

    for _, v in ipairs(cfg.files) do
        add_files(v)
    end
end
