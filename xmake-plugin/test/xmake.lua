task("test")
    set_category("plugin")

    on_run(function()
        import("core.project.project")
        import("core.project.task")

        for targetname, target in pairs(project.targets()) do
            if target:is_binary() and target:get("group") == "tests" then
                os.exec("xmake r " .. targetname)
            end
        end
    end)

    set_menu({
        usage = "xmake test",
        description = "Run all tests.",
        options = {},
    })
