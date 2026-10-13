#pragma once

#include <cstdint>

constexpr uint32_t rotr32(uint32_t x, uint8_t r) {
    return (x>>r)|(x<<((32u-r)&31u));
}

constexpr uint32_t rotl32(uint32_t x, uint8_t r) {
    return (x<<r)|(x>>((32u-r)&31u));
}

constexpr uint64_t rotr64(uint64_t x, uint8_t r) {
    return (x>>r)|(x<<((64u-r)&63u));
}

constexpr uint64_t rotl64(uint64_t x, uint8_t r) {
    return (x<<r)|(x>>((64u-r)&63u));
}

inline uint32_t XSH_RR(uint64_t state){
    uint8_t r=state>>59;
    uint32_t x=(((state>>18)^state)>>27);
    return rotr32(x, r);
}

inline uint64_t __(uint64_t state){
    return rotl64(state*5,7)*9;
}
