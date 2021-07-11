package("libnyquist")
    set_homepage("https://github.com/ddiakopoulos/libnyquist/")
    set_description(
        "Cross platform C++11 library for decoding audio (mp3, wav, ogg, opus,"
        .. " flac, etc)"
    )
    set_license("BSD-2-Clause")

    set_urls("https://github.com/ddiakopoulos/libnyquist.git")

    add_deps("cmake")
    add_links("libnyquist", "libwavpack")

    on_install("windows", "linux", "macosx", "mingw", function(pkg)
        import("package.tools.cmake").install(pkg, {"-DBUILD_EXAMPLE=OFF"})
        os.cp("include/libnyquist/*.h", pkg:installdir("include", "libnyquist"))
    end)

    on_test(function(pkg)
        assert(pkg:check_cxxsnippets(
            {
                test = [[
                    void test() {
                        nqr::NyquistIO loader;
                    }
                ]]
            },
            {
                configs = { languages = "c++11" },
                includes = "libnyquist/Decoders.h",
            }
        ))
    end)
