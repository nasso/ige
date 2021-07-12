set_project("ige")

add_rules("mode.debug", "mode.release")

add_plugindirs("xmake-plugin")

includes("ige")
includes("tests")
includes("examples")
