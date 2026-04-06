#include "mesh.hpp"

using namespace vox::ral;

bgfx::VertexLayout ChunkVertex::layout()
{
    static bgfx::VertexLayout layout;
    static bool initialized = false;

    if (initialized)
        return layout;

    spdlog::trace("Initializing vertex layout for ChunkVertex");

    layout.begin()
        .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
        .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
        .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
        .end();

    initialized = true;
    return layout;
}

Mesh::Mesh(std::span<const ChunkVertex> vertices, std::span<const uint16_t> indices)
{
    spdlog::trace("Creating mesh with {} vertices and {} indices", vertices.size(), indices.size());

    vertexBuffer = bgfx::createVertexBuffer(
        bgfx::makeRef(vertices.data(), vertices.size_bytes()),
        ChunkVertex::layout());

    if (!bgfx::isValid(vertexBuffer))
    {
        spdlog::error("Failed to create vertex buffer");
        vertexBuffer = bgfx::VertexBufferHandle{bgfx::kInvalidHandle};
    }

    indexBuffer = bgfx::createIndexBuffer(
        bgfx::makeRef(indices.data(), indices.size_bytes()));

    if (!bgfx::isValid(indexBuffer))
    {
        spdlog::error("Failed to create index buffer");
        indexBuffer = bgfx::IndexBufferHandle{bgfx::kInvalidHandle};
    }
}

Mesh::~Mesh()
{
    spdlog::trace("Destroying mesh");

    if (bgfx::isValid(vertexBuffer))
        bgfx::destroy(vertexBuffer);
    if (bgfx::isValid(indexBuffer))
        bgfx::destroy(indexBuffer);
}

void Mesh::submit(bgfx::ViewId view, bgfx::ProgramHandle program, uint64_t state) const
{
    if (!bgfx::isValid(vertexBuffer) || !bgfx::isValid(indexBuffer))
        return;

    bgfx::setVertexBuffer(0, vertexBuffer);
    bgfx::setIndexBuffer(indexBuffer);
    bgfx::submit(view, program, state);
}