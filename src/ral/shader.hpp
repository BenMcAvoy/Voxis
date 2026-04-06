#pragma once

#include <bgfx/bgfx.h>

#include <string_view>

namespace vox::ral
{
    class Shader
    {
    public:
        /// Creates a shader program from the given vertex and fragment shader files.
        /// When autoResolve is true, it expects a filename rather than a full path. It will
        /// look in `shaders/<backend>/` for the appropriate backend.
        Shader(std::string_view vsPath, std::string_view fsPath, bool autoResolve = true);

        ~Shader();

        bgfx::ProgramHandle getProgramHandle() const { return program; }

    private:
        bgfx::ProgramHandle program;
        bgfx::ShaderHandle vertexShader;
        bgfx::ShaderHandle fragmentShader;

        bgfx::ShaderHandle loadShader(std::string_view path);
    };
} // namespace vox::ral