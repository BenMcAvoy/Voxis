#include "ral/window.hpp"
#include "ral/shader.hpp"
#include "ral/uniform.hpp"
#include "ral/mesh.hpp"
#include "ral/image.hpp"
#include "ral/camera.hpp"
#include "ral/helpers.hpp"

#include "chunk.hpp"

#include <cstdlib>
#include <ctime>

constexpr auto WINDOW_WIDTH = 1280;
constexpr auto WINDOW_HEIGHT = 720;

int main()
{
    spdlog::set_level(spdlog::level::trace);

    vox::ral::Window window("Voxis", WINDOW_WIDTH, WINDOW_HEIGHT, vox::ral::RendererType::Vulkan);
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

    std::srand(static_cast<unsigned>(std::time(nullptr)));

    vox::Chunk chunk(1, 0);
    mesh = chunk.greedyMesh();

    bool wireframe = false;

    // Main loop
    while (window.nextFrame())
    {
        if (window.isKeyPressed(SDL_SCANCODE_F2))
        {
            // Pick a random face (0-5), then a random position on it
            int face = std::rand() % 6;
            int u = std::rand() % 16;
            int v = std::rand() % 16;

            int x, y, z;
            switch (face)
            {
            case 0: x =  0; y = u; z = v; break; // -X face
            case 1: x = 15; y = u; z = v; break; // +X face
            case 2: x = u; y =  0; z = v; break; // -Y face
            case 3: x = u; y = 15; z = v; break; // +Y face
            case 4: x = u; y = v; z =  0; break; // -Z face
            default: x = u; y = v; z = 15; break; // +Z face
            }

            chunk.setVoxel(x, y, z, {0, 0});
            mesh = chunk.greedyMesh();
            spdlog::info("Broke voxel at ({}, {}, {})", x, y, z);
        }

        if (window.isKeyPressed(SDL_SCANCODE_F1))
        {
            wireframe = !wireframe;
            spdlog::info("Wireframe mode {}", wireframe ? "enabled" : "disabled");
        }

        bgfx::setDebug(BGFX_DEBUG_TEXT | (wireframe ? BGFX_DEBUG_WIREFRAME : 0));

        camera.update(window.getAspectRatio(), &window);
        camera.submit(0);

        vox::ral::setTexture(0, s_texColor, whiteImage);
        mesh.submit(0, shader.getProgramHandle());

        bgfx::dbgTextPrintf(1, 1, 0x0f, "FPS: %.2f", window.getFPS());
    }
}