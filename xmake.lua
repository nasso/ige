set_project("ige")

add_rules("mode.debug", "mode.release")

add_plugindirs("xmake-plugin")
add_repositories("local-repo xmake-repo")

includes("ige")
includes("tests")
includes("examples")
