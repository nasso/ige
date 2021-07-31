target("glad")
    set_kind("object")
    add_files("src/**.c")
    add_includedirs("include", {public=true})
