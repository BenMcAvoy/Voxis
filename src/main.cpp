#include "ral/window.hpp"
#include "ral/shader.hpp"
#include "ral/uniform.hpp"
#include "ral/mesh.hpp"
#include "ral/image.hpp"
#include "ral/camera.hpp"
#include "ral/helpers.hpp"

constexpr auto WINDOW_WIDTH = 1280;
constexpr auto WINDOW_HEIGHT = 720;

int main()
{
    spdlog::set_level(spdlog::level::trace);

    vox::ral::Window window("Voxis", WINDOW_WIDTH, WINDOW_HEIGHT, vox::ral::RendererType::Direct3D12);
    vox::ral::Camera camera(window.getAspectRatio());

    // CCW triangle indices
    std::array<vox::ral::ChunkVertex, 4> vertices = {{
        {bx::Vec3{-0.5f, 0.5f, 0.0f}, 0xff0000ff, vox::ral::Vec2{0.0f, 0.0f}}, // top-left, red    (ABGR: A=ff R=ff G=00 B=00)
        {bx::Vec3{0.5f, 0.5f, 0.0f}, 0xff00ff00, vox::ral::Vec2{1.0f, 0.0f}},  // top-right, green  (ABGR: A=ff R=00 G=ff B=00)
        {bx::Vec3{0.5f, -0.5f, 0.0f}, 0xffff0000, vox::ral::Vec2{1.0f, 1.0f}}, // bottom-right, blue (ABGR: A=ff R=00 G=00 B=ff)
        {bx::Vec3{-0.5f, -0.5f, 0.0f}, 0xffffffff, vox::ral::Vec2{0.0f, 1.0f}} // bottom-left, white
    }};
    std::array<uint16_t, 6> indices{3, 2, 0, 2, 1, 0};

    vox::ral::Mesh mesh(vertices, indices);
    vox::ral::Shader shader("vert.bin", "frag.bin");
    vox::ral::Uniform s_texColor("s_texColor", bgfx::UniformType::Sampler);

    uint32_t white = 0xffffffff;
    vox::ral::Image whiteImage(&white, 1, 1);

    // Main loop
    while (window.nextFrame())
    {
        camera.update(window.getAspectRatio(), &window);
        camera.submit(0);

        vox::ral::setTexture(0, s_texColor, whiteImage);
        mesh.submit(0, shader.getProgramHandle());

        bgfx::dbgTextPrintf(1, 1, 0x0f, "FPS: %.2f", window.getFPS());
    }
}