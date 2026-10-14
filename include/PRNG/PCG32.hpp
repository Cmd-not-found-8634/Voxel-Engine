#pragma once

#include <cstdint>
#include <prng_utils.hpp>

class PCG32{
private:
    std::uint64_t state, inc;
    static constexpr std::uint64_t a=6364136223846793005ULL;
    static constexpr std::uint64_t a_inv=13877824140714322085ULL;
public:
    void reset(std::uint64_t seed, std::uint64_t stream){
        state=0;
        inc=(stream<<1)|1;
        state=state*a+inc;
        state+=seed;
        state=state*a+inc;
    }
    PCG32(std::uint64_t seed, std::uint64_t stream){
        reset(seed, stream);
    }
    std::uint32_t next(){
        std::uint32_t result=XSH_RR(state);
        state=state*a+inc;
        return result;
    }
    std::uint32_t prev(){
        state=(state-inc)*a_inv;
        return XSH_RR(state);
    }
    void advance(std::uint64_t delta) {
        std::uint64_t cur_mult=a, cur_plus=inc;
        std::uint64_t acc_mult=1, acc_plus=0;
        while(delta>0){
            if(delta&1){
                acc_mult*=cur_mult;
                acc_plus=acc_plus*cur_mult+cur_plus;
            }
            cur_plus=(cur_mult+1)*cur_plus;
            cur_mult*=cur_mult;
            delta>>=1;
        }
        state=acc_mult*state+acc_plus;
    }
    void rewind(std::uint64_t delta) {
        std::uint64_t cur_mult=a_inv, cur_plus=0-a_inv*inc;
        std::uint64_t acc_mult=1, acc_plus=0;
        while(delta>0){
            if(delta&1){
                acc_mult*=cur_mult;
                acc_plus=acc_plus*cur_mult+cur_plus;
            }
            cur_plus=(cur_mult+1)*cur_plus;
            cur_mult*=cur_mult;
            delta>>=1;
        }
        state=acc_mult*state+acc_plus;
    }
};