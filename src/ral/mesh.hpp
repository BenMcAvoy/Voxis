#pragma once

#include <bgfx/bgfx.h>
#include <bx/math.h>

#include <spdlog/spdlog.h>

#include <span>

namespace vox::ral
{
    struct Vec2
    {
        float x, y;
    };

    struct ChunkVertex
    {
        bx::Vec3 pos;
        uint32_t color;
        Vec2 uv;

        ChunkVertex(bx::Vec3 pos, uint32_t color, Vec2 uv) : pos(pos), color(color), uv(uv) {}

        static bgfx::VertexLayout layout();
    };

    class Mesh
    {
    public:
        Mesh(std::span<const ChunkVertex> vertices, std::span<const uint16_t> indices);
        ~Mesh();

        void submit(bgfx::ViewId view, bgfx::ProgramHandle program, uint64_t state = 0) const;

        // Don't allow copying (no RC on handles)
        Mesh(const Mesh &) = delete;
        Mesh &operator=(const Mesh &) = delete;

        Mesh(Mesh &&o) noexcept;
        Mesh &operator=(Mesh &&) noexcept;

    private:
        bgfx::VertexBufferHandle vertexBuffer;
        bgfx::IndexBufferHandle indexBuffer;
    };
};