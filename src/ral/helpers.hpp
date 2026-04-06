#include <bgfx/bgfx.h>

#include "uniform.hpp"
#include "image.hpp"

namespace vox::ral
{
    // bgfx::setTexture(0, s_texColor.getHandle(), whiteImage.getTextureHandle());
    void setTexture(int slot, Uniform &uniform, const Image &image);

    using RendererType = bgfx::RendererType::Enum;
    using UniformType = bgfx::UniformType::Enum;
}