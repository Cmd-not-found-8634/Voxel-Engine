#pragma once

#include <cstdint>

constexpr std::uint32_t rotr32(std::uint32_t x, std::uint8_t r) {
    r&=31u;
    return (x>>r)|(x<<((32u-r)&31u));
}

constexpr std::uint32_t rotl32(std::uint32_t x, std::uint8_t r) {
    r&=31u;
    return (x<<r)|(x>>((32u-r)&31u));
}

constexpr std::uint64_t rotr64(std::uint64_t x, std::uint8_t r) {
    r&=63u;
    return (x>>r)|(x<<((64u-r)&63u));
}

constexpr std::uint64_t rotl64(std::uint64_t x, std::uint8_t r) {
    r&=63u;
    return (x<<r)|(x>>((64u-r)&63u));
}

constexpr std::uint32_t XSH_RR(std::uint64_t state){
    std::uint8_t r=static_cast<std::uint8_t>(state>>59);
    std::uint32_t x=static_cast<std::uint32_t>(((state>>18)^state)>>27);
    return rotr32(x, r);
}

constexpr std::uint64_t star_star(std::uint64_t state){
    return rotl64(state*5,7)*9;
}

constexpr std::uint64_t uxsr_64(std::uint64_t x, std::uint8_t shift){
    for(std::uint8_t s=shift; s!=0 && s<64; s<<=1) x^=x>>s;
    return x;
}

constexpr std::uint64_t mix(std::uint64_t x){
    constexpr std::uint64_t MIX1 = 0xbf58476d1ce4e5b9ULL;
    constexpr std::uint64_t MIX2 = 0x94d049bb133111ebULL;
    x^=x>>30;
    x*=MIX1;
    x^=x>>27;
    x*=MIX2;
    x^=x>>31;
    return x;
}

constexpr std::uint64_t unmix(std::uint64_t x){
    std::uint64_t INV_MIX1 = 0x96de1b173f119089ULL;
    std::uint64_t INV_MIX2 = 0x319642b2d24d8ec3ULL;
    x=uxsr_64(x,31);
    x*=INV_MIX2;
    x=uxsr_64(x,27);
    x*=INV_MIX1;
    x=uxsr_64(x,30);
    return x;
}
