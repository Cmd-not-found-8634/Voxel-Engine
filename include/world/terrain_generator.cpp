#include <world/terrain_generator.hpp>

#include <iostream>

int main()
{
    const std::uint64_t worldSeed = 123456789ULL;

    TerrainGenerator terrain(worldSeed);

    // ------------------------------------------------------------
    // Directly query terrain.
    // ------------------------------------------------------------

    auto column =
        terrain.sampleColumn(1000, 2000);

    std::cout
        << "Height: "
        << column.height
        << '\n';

    // ------------------------------------------------------------
    // Query an individual block.
    // ------------------------------------------------------------

    auto block =
        terrain.blockAt(1000, 64, 2000);

    std::cout<<static_cast<int>(block)<<std::endl;

    // ------------------------------------------------------------
    // Generate one 16x16 chunk.
    // ------------------------------------------------------------

    auto blocks = terrain.generateChunk(62, 125);

    std::cout
        << "Generated "
        << blocks.size()
        << " blocks\n";
}