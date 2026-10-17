#pragma once

#include <cstdint>
#include <cmath>
#include <PRNG/prng_utils.hpp>

class PerlinNoise{
private:
    const std::uint64_t seed;
    inline static constexpr double D = 0.7071067811865475244;
    inline static constexpr double dir2d[8][2]={{1.0, 0.0}, {D, D}, {0.0, 1.0}, {-D, D}, {-1.0, 0.0}, {-D, -D}, {0.0, -1.0}, { D, -D}};
    inline static constexpr double dir3d[12][3]={{0.0, D, D}, {0.0, D, -D}, {0.0, -D, D}, {0.0, -D, -D}, {D, 0.0, D}, {D, 0.0, -D}, {-D, 0.0, D}, {-D, 0.0, -D}, {D, D, 0.0}, {D, -D, 0.0}, {-D, D, 0.0}, {-D, -D, 0.0}};
public:
    PerlinNoise(std::uint64_t _seed): seed(_seed) {}
    static constexpr double normalize(std::int64_t x, std::int64_t chunk_size){ // preferably use custom normalize
        return static_cast<double>(x)/static_cast<double>(chunk_size);
    }
    double noise2d(double x, double y) const{
        std::uint64_t c_x=static_cast<std::uint64_t>(std::floor(x));
        std::uint64_t c_y=static_cast<std::uint64_t>(std::floor(y));
        double r_x=x-floor(x);
        double r_y=y-floor(y);
        double grad0=get_dot_2d(c_x,c_y,r_x,r_y);
        double grad1=get_dot_2d(c_x,c_y+1,r_x,r_y-1);
        double grad2=get_dot_2d(c_x+1,c_y,r_x-1,r_y);
        double grad3=get_dot_2d(c_x+1,c_y+1,r_x-1,r_y-1);
        double f1=fade(r_x), f2=fade(r_y);
        return lerp(f1,lerp(f2,grad0,grad1),lerp(f2,grad2,grad3));
    }
    double noise3d(double x, double y, double z) const{
        std::uint64_t c_x=static_cast<std::uint64_t>(std::floor(x));
        std::uint64_t c_y=static_cast<std::uint64_t>(std::floor(y));
        std::uint64_t c_z=static_cast<std::uint64_t>(std::floor(z));
        double r_x=x-floor(x);
        double r_y=y-floor(y);
        double r_z=z-floor(z);
        double grad0=get_dot_3d(c_x+0,c_y+0,c_z+0,r_x-0,r_y-0,r_z-0);
        double grad1=get_dot_3d(c_x+0,c_y+0,c_z+1,r_x-0,r_y-0,r_z-1);
        double grad2=get_dot_3d(c_x+0,c_y+1,c_z+0,r_x-0,r_y-1,r_z-0);
        double grad3=get_dot_3d(c_x+0,c_y+1,c_z+1,r_x-0,r_y-1,r_z-1);
        double grad4=get_dot_3d(c_x+1,c_y+0,c_z+0,r_x-1,r_y-0,r_z-0);
        double grad5=get_dot_3d(c_x+1,c_y+0,c_z+1,r_x-1,r_y-0,r_z-1);
        double grad6=get_dot_3d(c_x+1,c_y+1,c_z+0,r_x-1,r_y-1,r_z-0);
        double grad7=get_dot_3d(c_x+1,c_y+1,c_z+1,r_x-1,r_y-1,r_z-1);
        double f1=fade(r_x), f2=fade(r_y), f3=fade(r_z);
        return lerp(f1,lerp(f2,lerp(f3,grad0,grad1),lerp(f3,grad2,grad3)),lerp(f2,lerp(f3,grad4,grad5),lerp(f3,grad6,grad7)));
    }
    constexpr std::uint64_t hash2(std::uint64_t x, std::uint64_t y) const{
        std::uint64_t h=mix(seed^0x243f6a8885a308d3ULL);
        h=mix(h^mix(x+0x9e3779b97f4a7c15ULL));
        h=mix(h^mix(y+0x3c6ef372fe94f82aULL));
        return h;
    }
    constexpr std::uint64_t hash3(std::uint64_t x, std::uint64_t y, std::uint64_t z) const{
        std::uint64_t h=mix(seed^0x13198a2e03707344ULL);
        h=mix(h^mix(x+0x9e3779b97f4a7c15ULL));
        h=mix(h^mix(y+0x3c6ef372fe94f82aULL));
        h=mix(h^mix(z+0xa54ff53a5f1d36f1ULL));
        return h;
    };
    constexpr double get_dot_2d(std::uint64_t c_x, std::uint64_t c_y, double r_x, double r_y) const{
        std::uint64_t ch=hash2(c_x,c_y)%(sizeof(dir2d)/sizeof(dir2d[0]));
        double grad=dir2d[ch][0]*r_x+dir2d[ch][1]*r_y;
        return grad;
    }
    constexpr double get_dot_3d(std::uint64_t c_x, std::uint64_t c_y, std::uint64_t c_z, double r_x, double r_y, double r_z) const{
        std::uint64_t ch=hash3(c_x,c_y,c_z)%(sizeof(dir3d)/sizeof(dir3d[0]));
        double grad=dir3d[ch][0]*r_x+dir3d[ch][1]*r_y+dir3d[ch][2]*r_z;
        return grad;
    }
    static constexpr double lerp(double t, double a, double b){
        return a+t*(b-a);
    }
    static constexpr double fade(double t){
        return t*t*t*(t*(t*6.0-15.0)+10.0);
    }
};
