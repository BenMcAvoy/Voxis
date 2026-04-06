#include "helpers.hpp"

void vox::ral::setTexture(int slot, Uniform &uniform, const Image &image)
{
    if (!bgfx::isValid(uniform.getHandle()) || !bgfx::isValid(image.getTextureHandle()))
        return;

    bgfx::setTexture(slot, uniform.getHandle(), image.getTextureHandle());
}