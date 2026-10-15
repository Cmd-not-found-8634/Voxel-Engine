#pragma once

#include <cstdint>
#include <PRNG/prng_utils.hpp>
#include <PRNG/SplitMix64.hpp>

class xoshiro256ss{
private:
    std::uint64_t s0,s1,s2,s3;
public:
    xoshiro256ss(std::uint64_t seed=0){
        reset(seed);
    }
    constexpr void reset(std::uint64_t seed){
        SplitMix64 seeder(seed);
        s0=seeder.next();
        s1=seeder.next();
        s2=seeder.next();
        s3=seeder.next();
    }
    constexpr std::uint64_t next(){
        std::uint64_t y=star_star(s1);
        std::uint64_t t=s1<<17;
        s2^=s0;
        s3^=s1;
        s1^=s2;
        s0^=s3;
        s2^=t;
        s3=rotl64(s3,45);
        return y;
    }
};
