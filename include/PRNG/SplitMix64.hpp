#pragma once

#include <cstdint>
#include <PRNG/prng_utils.hpp>

class SplitMix64{
private:
    std::uint64_t state, seed, index;
    static constexpr std::uint64_t GAMMA = 0x9e3779b97f4a7c15ULL;
    static constexpr std::uint64_t INV_GAMMA = 0xf1de83e19937733dULL;
public:
    explicit constexpr SplitMix64(std::uint64_t seed=0) :state(seed), seed(seed), index(0) {}

    constexpr std::uint64_t state_at(std::uint64_t index) const{
        return seed+(index+1)*GAMMA;
    }

    constexpr std::uint64_t value_at(std::uint64_t index) const{
        return mix(state_at(index));
    }

    constexpr std::uint64_t get_index() const{
        return index;
    }

    constexpr std::uint64_t next(){
        state+=GAMMA;
        ++index;
        return mix(state);
    }

    constexpr std::uint64_t previous(){
        std::uint64_t value=mix(state);
        --index;
        state-= GAMMA;
        return value;
    }

    constexpr void advance(std::uint64_t d){
        index+=d;
        state+=d*GAMMA;
    }

    constexpr void rewind(std::uint64_t d){
        index-=d;
        state-=d*GAMMA;
    }
    
    constexpr void reset(){
        index=0;
        state=seed;
    }
    
    constexpr void reset(std::uint64_t _seed){
        seed=_seed;
        index=0;
        state=_seed;
    }
    
    constexpr std::uint64_t get_seed() const{
        return seed;
    }
    
    static constexpr std::uint64_t previous_value(std::uint64_t value){
        std::uint64_t state=unmix(value);
        state-=GAMMA;
        return mix(state);
    }
    
    static constexpr std::uint64_t next_value(std::uint64_t value){
        std::uint64_t state=unmix(value);
        state+=GAMMA;
        return mix(state);
    }
    
    constexpr std::uint64_t state_from_value(std::uint64_t value) const{
        return unmix(value);
    }
    
    constexpr std::uint64_t index_from_state(std::uint64_t state) const{
        return (state-seed)*INV_GAMMA-1;
    }
    
    constexpr std::uint64_t index_from_value(std::uint64_t value) const{
        return index_from_state(state_from_value(value));
    }
    
    constexpr std::uint64_t value_from_state(std::uint64_t state) const{
        return mix(state);
    }
    
    constexpr std::uint64_t state_from_index(std::uint64_t index) const{
        return state_at(index);
    }
    
    constexpr std::uint64_t value_from_index(std::uint64_t index) const{
        return value_from_state(state_from_index(index));
    }
};

