#include "chunk.hpp"

#include <vector>
#include <array>

using namespace vox;

Chunk::Chunk(std::uint8_t defaultId, std::uint8_t defaultData)
{
    for (int x = 0; x < SIZE; x++)
    {
        for (int y = 0; y < SIZE; y++)
        {
            for (int z = 0; z < SIZE; z++)
            {
                voxels[x][y][z] = {defaultId, defaultData};
            }
        }
    }
}

void Chunk::setVoxel(int x, int y, int z, Voxel voxel)
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
        throw std::out_of_range("Voxel coordinates out of range");

    voxels[x][y][z] = voxel;
}

Voxel Chunk::getVoxel(int x, int y, int z) const
{
    if (x < 0 || x >= SIZE || y < 0 || y >= SIZE || z < 0 || z >= SIZE)
    {
        throw std::out_of_range("Voxel coordinates out of range");
    }

    return voxels[x][y][z];
}

ral::Mesh Chunk::greedyMesh() const
{
    std::vector<ral::ChunkVertex> vertices;
    std::vector<uint16_t> indices;

    // For each of the 6 face directions: +X, -X, +Y, -Y, +Z, -Z
    // Each direction is described by:
    //   normal axis (0=X,1=Y,2=Z), normal sign (+1/-1),
    //   and two tangent axes (u, v)
    struct FaceDir
    {
        int axis;   // normal axis
        int sign;   // +1 or -1
        int u, v;   // tangent axes
    };

    static constexpr std::array<FaceDir, 6> dirs = {{
        {0, +1, 1, 2}, // +X
        {0, -1, 1, 2}, // -X
        {1, +1, 2, 0}, // +Y  (Z×X = +Y, not X×Z = -Y)
        {1, -1, 2, 0}, // -Y
        {2, +1, 0, 1}, // +Z
        {2, -1, 0, 1}, // -Z
    }};

    for (const auto &dir : dirs)
    {
        // visited[u][v] marks whether the face at slice (u,v) has been merged
        bool visited[SIZE][SIZE] = {};

        // Sweep through each slice along the normal axis
        for (int d = 0; d < SIZE; d++)
        {
            // Reset visited for this slice
            for (int i = 0; i < SIZE; i++)
                for (int j = 0; j < SIZE; j++)
                    visited[i][j] = false;

            // Build face mask for this slice
            // mask[u][v] holds the voxel id if a visible face exists, else 0
            uint8_t mask[SIZE][SIZE] = {};

            for (int u = 0; u < SIZE; u++)
            {
                for (int v = 0; v < SIZE; v++)
                {
                    int coords[3];
                    coords[dir.axis] = d;
                    coords[dir.u]    = u;
                    coords[dir.v]    = v;

                    Voxel current = voxels[coords[0]][coords[1]][coords[2]];
                    if (current.id == 0)
                    {
                        mask[u][v] = 0;
                        continue;
                    }

                    // Check the neighbour in the normal direction
                    int nd = d + dir.sign;
                    if (nd >= 0 && nd < SIZE)
                    {
                        int nc[3];
                        nc[dir.axis] = nd;
                        nc[dir.u]    = u;
                        nc[dir.v]    = v;
                        if (voxels[nc[0]][nc[1]][nc[2]].id != 0)
                        {
                            mask[u][v] = 0; // face is hidden
                            continue;
                        }
                    }
                    mask[u][v] = current.id;
                }
            }

            // Greedy merge over the mask
            for (int u = 0; u < SIZE; u++)
            {
                for (int v = 0; v < SIZE; v++)
                {
                    if (!mask[u][v] || visited[u][v])
                        continue;

                    uint8_t id = mask[u][v];

                    // Expand in the v direction
                    int dv = 1;
                    while (v + dv < SIZE && mask[u][v + dv] == id && !visited[u][v + dv])
                        dv++;

                    // Expand in the u direction
                    int du = 1;
                    bool canExpand = true;
                    while (u + du < SIZE && canExpand)
                    {
                        for (int k = 0; k < dv; k++)
                        {
                            if (mask[u + du][v + k] != id || visited[u + du][v + k])
                            {
                                canExpand = false;
                                break;
                            }
                        }
                        if (canExpand) du++;
                    }

                    // Mark as visited
                    for (int eu = 0; eu < du; eu++)
                        for (int ev = 0; ev < dv; ev++)
                            visited[u + eu][v + ev] = true;

                    // Build the quad. The face sits at slice d (or d+1 for positive-normal faces).
                    // Corner positions in (axis, u-axis, v-axis) space:
                    float faceD = (float)(dir.sign > 0 ? d + 1 : d);

                    float p[4][3];
                    // bottom-left, bottom-right, top-right, top-left
                    float corners[4][2] = {
                        {(float)u,      (float)v     },
                        {(float)(u+du), (float)v     },
                        {(float)(u+du), (float)(v+dv)},
                        {(float)u,      (float)(v+dv)},
                    };

                    for (int c = 0; c < 4; c++)
                    {
                        p[c][dir.axis] = faceD;
                        p[c][dir.u]    = corners[c][0];
                        p[c][dir.v]    = corners[c][1];
                    }

                    uint32_t color = 0xff000000 | ((uint32_t)id * 0x36d7b7); // simple id→colour
                    auto baseIndex = (uint16_t)vertices.size();

                    for (int c = 0; c < 4; c++)
                    {
                        vertices.emplace_back(
                            bx::Vec3{p[c][0], p[c][1], p[c][2]},
                            color,
                            ral::Vec2{corners[c][0] - u, corners[c][1] - v}
                        );
                    }

                    // Two triangles; winding depends on face direction so normals face out
                    if (dir.sign > 0)
                    {
                        indices.push_back(baseIndex + 0);
                        indices.push_back(baseIndex + 1);
                        indices.push_back(baseIndex + 2);
                        indices.push_back(baseIndex + 0);
                        indices.push_back(baseIndex + 2);
                        indices.push_back(baseIndex + 3);
                    }
                    else
                    {
                        indices.push_back(baseIndex + 0);
                        indices.push_back(baseIndex + 2);
                        indices.push_back(baseIndex + 1);
                        indices.push_back(baseIndex + 0);
                        indices.push_back(baseIndex + 3);
                        indices.push_back(baseIndex + 2);
                    }
                }
            }
        }
    }

    return ral::Mesh(vertices, indices);
}