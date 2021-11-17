includes("framework")

local examples = {
    ["animation"] = { files = {"animation.cpp"} },
    ["audio_controls"] = { files = {"audio_controls.cpp"} },
    ["audio"] = { files = {"audio.cpp"} },
    ["bindings"] = { files = {"bindings.cpp"} },
    ["cubes"] = { files = {"cubes.cpp"} },
    ["gamepad"] = { files = {"gamepad.cpp"} },
    ["gltf"] = { files = {"gltf.cpp"} },
    ["hierarchy"] = { files = {"hierarchy.cpp"} },
    ["inputs"] = { files = {"inputs.cpp"} },
    ["mouse"] = { files = {"mouse.cpp"} },
    ["physics"] = { files = {"physics.cpp"} },
    ["script"] = { files = {"script.cpp"} },
    ["ui"] = { files = {"ui.cpp"} },
    ["skybox"] = { files = {"skybox.cpp"} },
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
