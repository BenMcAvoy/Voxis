#pragma once

#include <bgfx/bgfx.h>

#include <string_view>
#include <span>

namespace vox::ral
{
    class Image
    {
    public:
        Image(std::string_view path);
        Image(const void *data, int32_t width, int32_t height);

        ~Image();

        bgfx::TextureHandle getTextureHandle() const { return texture; }

    private:
        bgfx::TextureHandle texture;
    };
} // namespace vox::ral