add_requires(
    "glm ^0.9.9",
    "bullet3 ^3.09",
    "nlohmann_json ^3.9.1",
    "glfw ^3.3.4",
    "fx-gltf ^1.2.0",
    "libnyquist master"
)
add_requires("openal-soft ^1.21.1", {configs={shared=true}})

target("glad")
    set_kind("object")
    add_files("glad/src/**.c")
    add_includedirs("glad/include", {public=true})

target("stb")
    set_kind("object")
    add_includedirs("stb", {public=true})

target("ige")
    set_kind("static")
    set_languages("cxx20")
    set_warnings("all")

    add_rules("embed_bytes")

    -- source files + resources
    add_files("src/**.cpp")
    add_files("res/**", {rule="embed_bytes"})

    -- include directories
    add_includedirs("include", {public=true})
    add_includedirs("$(buildir)/ige")

    add_headerfiles("include/(**.hpp)")

    -- define some symbols
    add_defines(
        "_CRT_SECURE_NO_WARNINGS",
        "IGE_OPENGL"
    )

    -- define IGE_DEBUG when in debug mode
    if is_mode("debug") then
        add_defines("IGE_DEBUG")
    end

    -- dependencies
    add_deps("glad", "stb")
    add_packages(
        "glm",
        "bullet3",
        "glfw",
        "openal-soft",
        "nlohmann_json",
        "fx-gltf",
        "libnyquist",
        {public=true}
    )

rule("embed_bytes")
    before_build(function(target, opt)
        import("core.base.option")
        import("core.project.depend")
        import("private.utils.progress")

        --- generate the variable name for the given source path
        local function gen_ident(src_path)
            return path.relative(path.absolute(src_path), target:scriptdir())
                :gsub("%W", "_")
                :upper()
        end

        local function gen_header(dst_path, src_path)
            local file_name = path.filename(src_path)
            local var_name = gen_ident(src_path)

            progress.show(
                opt.progress,
                "${color.build.object}generating %s",
                dst_path
            )

            local src_file = assert(io.open(src_path, "rb"))
            local dst_file = assert(io.open(dst_path, "w"))

            local header_prefix = ""
                .. '#ifdef __cplusplus\n'
                .. 'extern "C" {\n'
                .. '#endif\n'
                .. 'static const char ' .. var_name .. '[] = {'

            local header_suffix = ""
                .. '0};\n'
                .. '#ifdef __cplusplus\n'
                .. '}\n'
                .. '#endif\n'

            dst_file:write(header_prefix)

            local buffer_size = 4096
            local bytes_left = src_file:size()

            -- write byte array
            while bytes_left > 0 do
                local read_size = math.min(bytes_left, buffer_size)
                local data = src_file:read(read_size)
                bytes_left = bytes_left - read_size

                local source = {}

                for i = 1, #data do
                    table.insert(source, string.format("%d,", data:byte(i)))
                end

                dst_file:write(table.concat(source))
            end

            dst_file:write(header_suffix)

            src_file:close()
            dst_file:close()
        end

        for _, sourcebatch in pairs(target:sourcebatches()) do
            if sourcebatch.rulename == "embed_bytes" then
                for _, src_path in pairs(sourcebatch.sourcefiles) do
                    local dst_path = path.join(
                        "$(buildir)",
                        target:name(),
                        path.relative(src_path, target:scriptdir()) .. ".h"
                    )

                    depend.on_changed(
                        function(target)
                            gen_header(dst_path, src_path)
                        end,
                        {
                            files = { src_path },
                            always_changed = option.get("rebuild")
                        }
                    )
                end
            end
        end
    end)
