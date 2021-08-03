set_project("ige")

add_rules("mode.asan", "mode.debug", "mode.releasedbg")

-- TODO officially support macosx
set_allowedplats("windows", "linux")
set_allowedarchs("windows|x64", "linux|x86_64")
set_allowedmodes("asan", "debug", "releasedbg")

set_defaultmode("debug")
set_defaultarchs("windows|x64", "linux|x86_64")

includes("ige")
includes("tests")
includes("examples")
