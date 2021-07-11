target("blobs")
    set_kind("static")
    add_rules("static_bytes")
    add_files("shaders/gl/**.glsl", {rule="static_bytes"})
    add_includedirs("$(buildir)/static_bytes", {interface=true})

rule("static_bytes")
    before_build(function(target, opt)
        import("core.base.option")
        import("core.project.depend")
        import("private.utils.progress")

        --- generate the variable name for the given source path
        local function gen_ident(src_path)
            local rel_src_path =
                path.relative(path.absolute(src_path), target:scriptdir())

            return (target:name() .. "_" .. rel_src_path)
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
            if sourcebatch.rulename == "static_bytes" then
                for _, src_path in pairs(sourcebatch.sourcefiles) do
                    local dst_path = path.join(
                        "$(buildir)",
                        "static_bytes",
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
