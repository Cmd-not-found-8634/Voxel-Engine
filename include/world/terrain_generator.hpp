#pragma once

#include <PRNG/prng_utils.hpp>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

class TerrainGenerator
{
public:
    using i64 = std::int64_t;
    using u64 = std::uint64_t;

    enum class block : std::uint8_t
    {
        Air,
        Stone,
        Dirt,
        Grass,
        Sand,
        Water,
        Snow
    };

    enum class Biome : std::uint8_t
    {
        Ocean,
        Plains,
        Forest,
        Desert,
        Mountains,
        SnowMountains
    };

    struct Config
    {
        int chunkSize = 16;

        int minY = 32;
        int maxY = 96;

        int seaLevel = 62;

        // Main terrain scales.
        double continentalScale = 0.00075;
        double hillScale         = 0.0060;
        double mountainScale     = 0.0024;

        // Number of FBM layers.
        int terrainOctaves = 5;
    };

    struct Column
    {
        int height;
        Biome biome;

        double temperature;
        double humidity;
        double mountain;
        double continentalness;
    };

public:

    explicit TerrainGenerator(u64 seed)
        : seed_(seed)
    {
    }

    TerrainGenerator(u64 seed, Config config)
        : seed_(seed),
          config_(config)
    {
    }

    // ------------------------------------------------------------
    // Public terrain API
    // ------------------------------------------------------------

    Column sampleColumn(i64 x, i64 z) const
    {
        // --------------------------------------------------------
        // 1. Continentalness
        //
        // Very low frequency.
        // Determines large-scale oceans and continents.
        // --------------------------------------------------------

        const double continentalRaw =
            fbm(
                x * config_.continentalScale,
                z * config_.continentalScale,
                config_.terrainOctaves,
                2.0,
                0.5,
                0xA17C9E37ULL
            );

        const double continentalness =
            smooth01(
                (continentalRaw - 0.40) / 0.22
            );

        // --------------------------------------------------------
        // 2. Hills
        // --------------------------------------------------------

        const double hills =
            fbm(
                x * config_.hillScale,
                z * config_.hillScale,
                5,
                2.0,
                0.5,
                0xB4D21F53ULL
            );

        // --------------------------------------------------------
        // 3. Mountains
        //
        // Ridged noise produces long sharp ridges.
        // --------------------------------------------------------

        const double mountainNoise =
            ridgedFbm(
                x * config_.mountainScale,
                z * config_.mountainScale,
                5,
                2.0,
                0.5,
                0xC81D4A29ULL
            );

        // Mountains should mostly occur inland.
        const double mountainMask =
            smooth01(
                (continentalness - 0.55) / 0.30
            );

        const double mountain =
            std::pow(mountainNoise, 1.8) *
            mountainMask;

        // --------------------------------------------------------
        // 4. Erosion-like noise
        //
        // Breaks up the otherwise uniform mountains.
        // --------------------------------------------------------

        const double erosion =
            fbm(
                x * 0.010,
                z * 0.010,
                4,
                2.0,
                0.5,
                0xD19E2B77ULL
            );

        // --------------------------------------------------------
        // 5. Combine the terrain
        // --------------------------------------------------------

        double height = 43.0;

        // Continental elevation.
        height += continentalness * 25.0;

        // Rolling hills.
        height +=
            (hills - 0.5) *
            10.0 *
            continentalness;

        // Mountains.
        height +=
            mountain *
            (20.0 + 35.0 * (1.0 - erosion));

        // Deep ocean floor.
        if (continentalness < 0.18)
        {
            height =
                40.0 +
                continentalness * 12.0 +
                (hills - 0.5) * 3.0;
        }

        const int finalHeight =
            std::clamp(
                static_cast<int>(std::lround(height)),
                config_.minY + 1,
                config_.maxY - 1
            );

        // --------------------------------------------------------
        // 6. Climate
        // --------------------------------------------------------

        const double temperature =
            fbm(
                x * 0.0015,
                z * 0.0015,
                4,
                2.0,
                0.5,
                0x11223344ULL
            );

        const double humidity =
            fbm(
                x * 0.0018,
                z * 0.0018,
                4,
                2.0,
                0.5,
                0x55667788ULL
            );

        // --------------------------------------------------------
        // 7. Biome
        // --------------------------------------------------------

        Biome biome;

        if (finalHeight <= config_.seaLevel)
        {
            biome = Biome::Ocean;
        }
        else if (mountain > 0.72)
        {
            if (temperature < 0.30)
                biome = Biome::SnowMountains;
            else
                biome = Biome::Mountains;
        }
        else if (temperature > 0.65 && humidity < 0.35)
        {
            biome = Biome::Desert;
        }
        else if (humidity > 0.63)
        {
            biome = Biome::Forest;
        }
        else
        {
            biome = Biome::Plains;
        }

        return {
            finalHeight,
            biome,
            temperature,
            humidity,
            mountain,
            continentalness
        };
    }

    // ------------------------------------------------------------
    // Get a single block in world space.
    // ------------------------------------------------------------

    block blockAt(i64 x, int y, i64 z) const
    {
        const Column column = sampleColumn(x, z);

        return blockAtColumn(
            x,
            y,
            z,
            column
        );
    }

    // ------------------------------------------------------------
    // Generate a complete chunk.
    //
    // chunkX/chunkZ are chunk coordinates, not block coordinates.
    //
    // Example:
    //   chunk (0,0) -> blocks [0..15]
    //   chunk (1,0) -> blocks [16..31]
    // ------------------------------------------------------------

    std::vector<block> generateChunk(
        i64 chunkX,
        i64 chunkZ
    ) const
    {
        const int S = config_.chunkSize;
        const int H = config_.maxY - config_.minY;

        std::vector<block> blocks(
            static_cast<std::size_t>(S) *
            static_cast<std::size_t>(H) *
            static_cast<std::size_t>(S),
            block::Air
        );

        auto index =
            [S, H](int x, int y, int z) -> std::size_t
        {
            return
                (static_cast<std::size_t>(x) *
                 static_cast<std::size_t>(H) +
                 static_cast<std::size_t>(y)) *
                static_cast<std::size_t>(S) +
                static_cast<std::size_t>(z);
        };

        // --------------------------------------------------------
        // First calculate the 16x16 column data.
        //
        // This is important for performance:
        // we do NOT recalculate the terrain height once per block.
        // --------------------------------------------------------

        std::vector<Column> columns(
            static_cast<std::size_t>(S) *
            static_cast<std::size_t>(S)
        );

        for (int lx = 0; lx < S; ++lx)
        {
            for (int lz = 0; lz < S; ++lz)
            {
                const i64 worldX =
                    chunkX * static_cast<i64>(S) + lx;

                const i64 worldZ =
                    chunkZ * static_cast<i64>(S) + lz;

                columns[
                    static_cast<std::size_t>(lx) * S + lz
                ] = sampleColumn(
                    worldX,
                    worldZ
                );
            }
        }

        // --------------------------------------------------------
        // Fill blocks.
        // --------------------------------------------------------

        for (int lx = 0; lx < S; ++lx)
        {
            for (int lz = 0; lz < S; ++lz)
            {
                const i64 worldX =
                    chunkX * static_cast<i64>(S) + lx;

                const i64 worldZ =
                    chunkZ * static_cast<i64>(S) + lz;

                const Column& column =
                    columns[
                        static_cast<std::size_t>(lx) * S + lz
                    ];

                for (int y = config_.minY;
                     y < config_.maxY;
                     ++y)
                {
                    blocks[
                        index(
                            lx,
                            y - config_.minY,
                            lz
                        )
                    ] =
                        blockAtColumn(
                            worldX,
                            y,
                            worldZ,
                            column
                        );
                }
            }
        }

        return blocks;
    }

private:

    u64 seed_;
    Config config_{};

    // ============================================================
    // Basic interpolation
    // ============================================================

    static double smooth(double t)
    {
        // Hermite interpolation:
        //
        // 3t^2 - 2t^3
        //
        // Gives zero derivative at the lattice boundaries.
        return t * t * (3.0 - 2.0 * t);
    }

    static double smooth01(double x)
    {
        x = std::clamp(x, 0.0, 1.0);
        return smooth(x);
    }

    // ============================================================
    // Deterministic 2D hash
    //
    // This is where your reversible PRNG is used.
    //
    // Same:
    //     seed + x + z + salt
    //
    // always produces the same value.
    //
    // Different noise layers use different salts.
    // ============================================================

    static u64 hash2D(
        u64 seed,
        i64 x,
        i64 z,
        u64 salt
    )
    {
        const u64 ux =
            static_cast<u64>(x);

        const u64 uz =
            static_cast<u64>(z);

        const u64 a =
            mix(
                ux +
                0x9e3779b97f4a7c15ULL +
                salt
            );

        const u64 b =
            mix(
                uz +
                0xbf58476d1ce4e5b9ULL +
                salt * 3ULL
            );

        // Rotate 32 bits.
        const u64 rotatedB =
            (b << 32) | (b >> 32);

        return mix(
            seed ^
            a ^
            rotatedB ^
            salt
        );
    }

    // ============================================================
    // u64 -> [0,1)
    // ============================================================

    static double random01(
        u64 seed,
        i64 x,
        i64 z,
        u64 salt
    )
    {
        const u64 h =
            hash2D(
                seed,
                x,
                z,
                salt
            );

        // 53 random bits -> double in [0,1)
        return
            static_cast<double>(h >> 11) *
            (1.0 / 9007199254740992.0);
    }

    // ============================================================
    // 2D value noise
    //
    // Four random lattice points:
    //
    // A ---- B
    // |      |
    // |      |
    // C ---- D
    //
    // Then bilinear interpolation.
    // ============================================================

    double valueNoise2D(
        double x,
        double z,
        u64 salt
    ) const
    {
        const i64 x0 =
            static_cast<i64>(std::floor(x));

        const i64 z0 =
            static_cast<i64>(std::floor(z));

        const double fx =
            x - static_cast<double>(x0);

        const double fz =
            z - static_cast<double>(z0);

        const double tx = smooth(fx);
        const double tz = smooth(fz);

        const double A =
            random01(seed_, x0,     z0,     salt);

        const double B =
            random01(seed_, x0 + 1, z0,     salt);

        const double C =
            random01(seed_, x0,     z0 + 1, salt);

        const double D =
            random01(seed_, x0 + 1, z0 + 1, salt);

        const double AB =
            A + (B - A) * tx;

        const double CD =
            C + (D - C) * tx;

        return
            AB + (CD - AB) * tz;
    }

    // ============================================================
    // FBM
    //
    // Several noise frequencies are added together.
    //
    // Large octaves -> large landforms
    // Small octaves -> detail
    // ============================================================

    double fbm(
        double x,
        double z,
        int octaves,
        double lacunarity,
        double gain,
        u64 salt
    ) const
    {
        double result = 0.0;

        double amplitude = 1.0;
        double frequency = 1.0;

        double amplitudeSum = 0.0;

        for (int i = 0; i < octaves; ++i)
        {
            const u64 octaveSalt =
                salt +
                static_cast<u64>(i) *
                0x9e3779b9ULL;

            result +=
                valueNoise2D(
                    x * frequency,
                    z * frequency,
                    octaveSalt
                ) *
                amplitude;

            amplitudeSum += amplitude;

            frequency *= lacunarity;
            amplitude *= gain;
        }

        return result / amplitudeSum;
    }

    /* ============================================================
    // Ridged FBM
    //
    // Normal noise:
    //
    //   /\    /\
    //  /  \__/  \
    //
    // Ridged:
    //
    //   /\  /\ /\
    //  /  \/  V  \
    //
    // Very useful for mountains.
    // ==========================================================*/

    double ridgedFbm(
        double x,
        double z,
        int octaves,
        double lacunarity,
        double gain,
        u64 salt
    ) const
    {
        double result = 0.0;

        double amplitude = 1.0;
        double frequency = 1.0;

        double amplitudeSum = 0.0;

        for (int i = 0; i < octaves; ++i)
        {
            const u64 octaveSalt =
                salt +
                static_cast<u64>(i) *
                0x9e3779b9ULL;

            const double n =
                valueNoise2D(
                    x * frequency,
                    z * frequency,
                    octaveSalt
                );

            // Turn valleys into ridges.
            const double ridge =
                1.0 -
                std::abs(
                    2.0 * n - 1.0
                );

            result +=
                ridge * ridge *
                amplitude;

            amplitudeSum += amplitude;

            frequency *= lacunarity;
            amplitude *= gain;
        }

        return result / amplitudeSum;
    }

    // ============================================================
    // 3D hash
    // ============================================================

    static u64 hash3D(
        u64 seed,
        i64 x,
        i64 y,
        i64 z,
        u64 salt
    )
    {
        u64 h =
            seed ^ salt;

        h ^=
            mix(
                static_cast<u64>(x) +
                0x9e3779b97f4a7c15ULL +
                salt
            );

        h ^=
            mix(
                static_cast<u64>(y) +
                0xbf58476d1ce4e5b9ULL +
                salt * 5ULL
            );

        h ^=
            mix(
                static_cast<u64>(z) +
                0x94d049bb133111ebULL +
                salt * 7ULL
            );

        return
            mix(h);
    }

    static double random03(
        u64 seed,
        i64 x,
        i64 y,
        i64 z,
        u64 salt
    )
    {
        const u64 h =
            hash3D(
                seed,
                x,
                y,
                z,
                salt
            );

        return
            static_cast<double>(h >> 11) *
            (1.0 / 9007199254740992.0);
    }

    // ============================================================
    // 3D value noise
    //
    // Used for caves.
    // ============================================================

    double valueNoise3D(
        double x,
        double y,
        double z,
        u64 salt
    ) const
    {
        const i64 x0 =
            static_cast<i64>(std::floor(x));

        const i64 y0 =
            static_cast<i64>(std::floor(y));

        const i64 z0 =
            static_cast<i64>(std::floor(z));

        const double tx =
            smooth(
                x - static_cast<double>(x0)
            );

        const double ty =
            smooth(
                y - static_cast<double>(y0)
            );

        const double tz =
            smooth(
                z - static_cast<double>(z0)
            );

        double c[2][2][2];

        for (int dx = 0; dx < 2; ++dx)
        {
            for (int dy = 0; dy < 2; ++dy)
            {
                for (int dz = 0; dz < 2; ++dz)
                {
                    c[dx][dy][dz] =
                        random03(
                            seed_,
                            x0 + dx,
                            y0 + dy,
                            z0 + dz,
                            salt
                        );
                }
            }
        }

        // Interpolate X.
        const double x00 =
            c[0][0][0] +
            (c[1][0][0] - c[0][0][0]) * tx;

        const double x10 =
            c[0][1][0] +
            (c[1][1][0] - c[0][1][0]) * tx;

        const double x01 =
            c[0][0][1] +
            (c[1][0][1] - c[0][0][1]) * tx;

        const double x11 =
            c[0][1][1] +
            (c[1][1][1] - c[0][1][1]) * tx;

        // Interpolate Y.
        const double y0v =
            x00 + (x10 - x00) * ty;

        const double y1v =
            x01 + (x11 - x01) * ty;

        // Interpolate Z.
        return
            y0v + (y1v - y0v) * tz;
    }

    // ============================================================
    // Convert column information into a block.
    // ============================================================

    block blockAtColumn(
        i64 x,
        int y,
        i64 z,
        const Column& column
    ) const
    {
        // Above terrain.
        if (y > column.height)
        {
            // Water fills terrain depressions.
            if (y <= config_.seaLevel)
                return block::Water;

            return block::Air;
        }

        // --------------------------------------------------------
        // Bedrock-ish bottom layer.
        // --------------------------------------------------------

        if (y <= config_.minY + 2)
            return block::Stone;

        // --------------------------------------------------------
        // Caves
        //
        // Only test underground blocks.
        // --------------------------------------------------------

        if (y > config_.minY + 5 &&
            y < column.height - 4)
        {
            const double cave =
                valueNoise3D(
                    x * 0.045,
                    y * 0.055,
                    z * 0.045,
                    0xE4F71B93ULL
                );

            if (cave > 0.71)
                return block::Air;
        }

        // --------------------------------------------------------
        // Shore/beach.
        // --------------------------------------------------------

        const bool nearSea =
            std::abs(
                column.height -
                config_.seaLevel
            ) <= 2;

        if (nearSea &&
            y >= column.height - 3)
        {
            return block::Sand;
        }

        // --------------------------------------------------------
        // Surface blocks based on biome.
        // --------------------------------------------------------

        if (y == column.height)
        {
            switch (column.biome)
            {
                case Biome::Desert:
                    return block::Sand;

                case Biome::SnowMountains:
                    return block::Snow;

                default:
                    return block::Grass;
            }
        }

        // --------------------------------------------------------
        // Topsoil.
        // --------------------------------------------------------

        if (y >= column.height - 3)
        {
            if (column.biome == Biome::Desert)
                return block::Sand;

            return block::Dirt;
        }

        return block::Stone;
    }
};