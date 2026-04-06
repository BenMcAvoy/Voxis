#include "uniform.hpp"

using namespace vox::ral;

Uniform::Uniform(std::string_view name, bgfx::UniformType::Enum type)
{
    std::string_view typeName;
    switch (type)
    {
    case bgfx::UniformType::Sampler:
        typeName = "Sampler";
        break;
    case bgfx::UniformType::Vec4:
        typeName = "Vec4";
        break;
    case bgfx::UniformType::Mat3:
        typeName = "Mat3";
        break;
    case bgfx::UniformType::Mat4:
        typeName = "Mat4";
        break;
    default:
        typeName = "Unknown";
        break;
    }

    spdlog::trace("Creating uniform '{}' of type {}", name, typeName);

    handle = bgfx::createUniform(name.data(), type);
    if (!bgfx::isValid(handle))
    {
        spdlog::error("Failed to create uniform '{}'", name);
        handle = BGFX_INVALID_HANDLE;
    }
}

Uniform::~Uniform()
{
    spdlog::trace("Destroying uniform");

    if (bgfx::isValid(handle))
        bgfx::destroy(handle);
}