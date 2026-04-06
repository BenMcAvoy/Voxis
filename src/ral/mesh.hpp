#pragma once

#include <bgfx/bgfx.h>

#include <glm/glm.hpp>

#include <spdlog/spdlog.h>

#include <span>

namespace vox::ral
{
    struct ChunkVertex
    {
        glm::vec3 pos;
        uint32_t color;
        glm::vec2 uv;

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