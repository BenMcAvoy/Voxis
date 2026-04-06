add_rules("mode.debug", "mode.release")

set_languages("c++23")
set_runtimes(is_mode("debug") and "MTd" or "MT")

package("imgui-fork")
    set_homepage("https://github.com/gamecoder-nz/imgui")
    set_description("gamecoder-nz fork of Dear ImGui")

    on_install(function (package)
        local srcdir = package:cachedir() .. "/imgui-fork-src"
        if not os.isdir(srcdir) then
            os.vrun("git clone --depth=1 https://github.com/gamecoder-nz/imgui.git " .. srcdir)
        end
        os.cp(srcdir .. "/*.cpp", ".")
        os.cp(srcdir .. "/*.h", ".")
        io.writefile("xmake.lua", [[
            add_rules("mode.debug", "mode.release")
            set_runtimes("$(runtimes)")
            target("imgui")
                set_kind("static")
                add_files("imgui.cpp", "imgui_demo.cpp", "imgui_draw.cpp", "imgui_tables.cpp", "imgui_widgets.cpp")
                add_headerfiles("*.h")
        ]])
        import("package.tools.xmake").install(package, {runtimes = package:config("runtimes")})
    end)
package_end()

add_requires("bgfx 8752", {configs = {vs = "2022", debug = is_mode("debug"), runtimes = is_mode("debug") and "MTd" or "MT"}})
add_requires("libsdl3", {configs = {shared = true}})
add_requires("stb")
add_requires("freetype")
add_requires("imgui-fork")
add_requires("spdlog")

target("Voxis")
    set_kind("binary")
    add_files("src/**.cpp")
    add_packages("bgfx", "libsdl3", "stb", "freetype", "imgui-fork", "spdlog")
    add_includedirs("src")

    if is_mode("debug") then
        add_defines("BX_CONFIG_DEBUG=1")
        add_defines("BGFX_CONFIG_DEBUG=1")
    else
        add_defines("BX_CONFIG_DEBUG=0")
        add_defines("BGFX_CONFIG_DEBUG=0")
    end

    before_build(function (target)
        local shaderdir = path.join(target:scriptdir(), "src", "shaders")
        local local_shader_include = path.join(shaderdir, "bgfx_shader.sh")

        if not os.isfile(local_shader_include) then
            local candidates = os.files(path.join(os.getenv("LOCALAPPDATA"), ".xmake", "cache", "packages", "*", "b", "bgfx", "*", "source", "bgfx", "src", "bgfx_shader.sh"))
            assert(#candidates > 0, "bgfx_shader.sh not found in xmake cache")
            os.cp(candidates[1], local_shader_include)
            print("[shaderc] copied bgfx_shader.sh to local shader directory")
        end

        local bgfx = assert(target:pkg("bgfx"), "bgfx package is not available")
        local shaderc = path.join(bgfx:installdir(), "bin", is_mode("debug") and "shadercDebug.exe" or "shadercRelease.exe")
        if not os.isexec(shaderc) then
            shaderc = path.join(bgfx:installdir(), "bin", "shadercRelease.exe")
        end
        assert(os.isexec(shaderc), "shaderc executable not found: " .. shaderc)

        local backends = {
            dx11   = { platform = "windows", profile = "s_5_0" },
            dx12   = { platform = "windows", profile = "s_5_0" },
            vulkan = { platform = "windows", profile = "spirv"  },
            opengl = { platform = "linux",   profile = "430"    },
            metal  = { platform = "osx",     profile = "metal"  },
        }

        local function compile_shader(backend, kind, input_name, output_name)
            local cfg = assert(backends[backend], "unknown shader backend: " .. backend)
            local input_path  = path.join(shaderdir, input_name)
            local output_path = path.join(target:targetdir(), "shaders", backend, output_name)
            local varying_path = path.join(shaderdir, "varying.def.sc")
            os.mkdir(path.directory(output_path))
            print(string.format("[shaderc:%s] %s -> %s", backend, input_name, output_name))
            os.execv(shaderc, {
                "-f", input_path,
                "-o", output_path,
                "--type", kind,
                "--platform", cfg.platform,
                "-p", cfg.profile,
                "--varyingdef", varying_path,
                "-i", shaderdir
            })
        end

        for backend in pairs(backends) do
            compile_shader(backend, "vertex",   "vert.sc",  "vert.bin")
            compile_shader(backend, "fragment", "frag.sc",  "frag.bin")
        end
    end)