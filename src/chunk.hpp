#include <cstdint>

#include "ral/mesh.hpp"

namespace vox
{
    struct Voxel
    {
        std::uint8_t id;
        std::uint8_t data;
    };
    static_assert(sizeof(Voxel) == 2, "Voxel struct should be 2 bytes");

    class Chunk
    {
    public:
        Chunk() = default;

        Chunk(std::uint8_t defaultId, std::uint8_t defaultData = 0);

        Voxel getVoxel(int x, int y, int z) const;

        ral::Mesh greedyMesh() const;

    private:
        static constexpr int SIZE = 16;
        Voxel voxels[SIZE][SIZE][SIZE];
    };
}