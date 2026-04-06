#include "image.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include <spdlog/spdlog.h>

using namespace vox::ral;

Image::Image(std::string_view path)
{
    spdlog::trace("Loading image from '{}'", path);

    int32_t width, height, channels;
    stbi_uc *data = stbi_load(path.data(), &width, &height, &channels, STBI_rgb_alpha);
    if (!data)
    {
        spdlog::error("Failed to load image '{}': {}", path, stbi_failure_reason());
        texture = BGFX_INVALID_HANDLE;
        return;
    }

    texture = bgfx::createTexture2D(
        static_cast<uint16_t>(width),
        static_cast<uint16_t>(height),
        false,
        1,
        bgfx::TextureFormat::RGBA8,
        BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP,
        bgfx::copy(data, width * height * STBI_rgb_alpha)); // 4 channels

    stbi_image_free(data);

    if (!bgfx::isValid(texture))
    {
        spdlog::error("Failed to create texture from image '{}'", path);
        texture = BGFX_INVALID_HANDLE;
    }
}

Image::Image(const void *data, int32_t width, int32_t height)
{
    spdlog::trace("Creating image from raw data ({}x{})", width, height);

    texture = bgfx::createTexture2D(
        static_cast<uint16_t>(width),
        static_cast<uint16_t>(height),
        false,
        1,
        bgfx::TextureFormat::RGBA8,
        BGFX_SAMPLER_POINT | BGFX_SAMPLER_UVW_CLAMP,
        bgfx::copy(data, width * height * 4)); // 4 channels

    if (!bgfx::isValid(texture))
    {
        spdlog::error("Failed to create texture from raw data");
        texture = BGFX_INVALID_HANDLE;
    }
}

Image::~Image()
{
    spdlog::trace("Destroying image");

    if (bgfx::isValid(texture))
        bgfx::destroy(texture);
}