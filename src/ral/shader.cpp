#include "shader.hpp"

#include <fstream>

#include <spdlog/spdlog.h>

#include <vector>

using namespace vox::ral;

Shader::Shader(std::string_view vsPath, std::string_view fsPath, bool autoResolve)
{
    if (autoResolve && (vsPath.contains('/') || vsPath.contains('\\') || fsPath.contains('/') || fsPath.contains('\\')))
    {
        spdlog::error("Auto-resolve is enabled but shader paths contain directory separators");
        throw std::invalid_argument("Shader paths must not contain directory separators when autoResolve is true");
    }

    if (autoResolve)
    {
        const char *backendDir;
        switch (bgfx::getRendererType())
        {
        case bgfx::RendererType::Direct3D11:
            backendDir = "dx11";
            break;
        case bgfx::RendererType::Direct3D12:
            backendDir = "dx12";
            break;
        case bgfx::RendererType::OpenGL:
            backendDir = "opengl";
            break;
        case bgfx::RendererType::OpenGLES:
            backendDir = "opengles";
            break;
        case bgfx::RendererType::Vulkan:
            backendDir = "vulkan";
            break;
        default:
            spdlog::error("Unsupported renderer type for shader auto-resolve");
            throw std::runtime_error("Unsupported renderer type for shader auto-resolve");
        }

        std::string vsFullPath = std::string("shaders/") + backendDir + "/" + vsPath.data();
        std::string fsFullPath = std::string("shaders/") + backendDir + "/" + fsPath.data();

        spdlog::trace("Auto-resolving shader paths to '{}' and '{}'", vsFullPath, fsFullPath);

        vsPath = vsFullPath;
        fsPath = fsFullPath;
    }

    spdlog::trace("Creating shader program from '{}' and '{}'", vsPath, fsPath);

    vertexShader = loadShader(vsPath);
    if (!bgfx::isValid(vertexShader))
    {
        spdlog::error("Failed to load vertex shader from '{}'", vsPath);
        throw std::runtime_error("Failed to load vertex shader");
    }

    fragmentShader = loadShader(fsPath);
    if (!bgfx::isValid(fragmentShader))
    {
        spdlog::error("Failed to load fragment shader from '{}'", fsPath);
        bgfx::destroy(vertexShader);
        throw std::runtime_error("Failed to load fragment shader");
    }

    program = bgfx::createProgram(vertexShader, fragmentShader, true);
    if (!bgfx::isValid(program))
    {
        spdlog::error("Failed to create shader program from '{}' and '{}'", vsPath, fsPath);
        bgfx::destroy(vertexShader);
        bgfx::destroy(fragmentShader);
        throw std::runtime_error("Failed to create shader program");
    }
}

Shader::~Shader()
{
    spdlog::trace("Destroying shader program");

    if (bgfx::isValid(program))
        bgfx::destroy(program);
}

bgfx::ShaderHandle Shader::loadShader(std::string_view path)
{
    spdlog::trace("Loading shader from '{}'", path);

    std::ifstream file(path.data(), std::ios::binary);
    if (!file)
    {
        spdlog::error("Failed to open shader file '{}'", path);
        return BGFX_INVALID_HANDLE;
    }

    std::vector<char> data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    return bgfx::createShader(bgfx::copy(data.data(), data.size()));
}