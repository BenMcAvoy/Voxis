#pragma once

#include <bgfx/bgfx.h>

#include <string_view>

#include <spdlog/spdlog.h>

namespace vox::ral
{
    class Uniform
    {
    public:
        Uniform(std::string_view name, bgfx::UniformType::Enum type);
        ~Uniform();

        bgfx::UniformHandle getHandle() const { return handle; }

        template <typename T>
        void set(const T &value)
        {
            if (!bgfx::isValid(handle))
                return;

            bgfx::setUniform(handle, &value);
        }

    private:
        bgfx::UniformHandle handle;
    };
} // namespace vox::ral