#include <iostream>
#include <PRNG/prng.hpp>

int main(){
    std::cout<<std::hex;

    // PCG32
    std::cout<<"PCG32(42,54)\n";
    PCG32 gen1(42,54);
    for(int i=0;i<5;i++) std::cout<<std::hex<<gen1.next()<<"\n";
    for(int i=0;i<5;i++) std::cout<<std::hex<<gen1.prev()<<"\n";
    gen1.advance(5);
    std::cout<<gen1.next()<<"\n";
    gen1.rewind(5);
    std::cout<<gen1.prev()<<"\n";
    std::cout<<"\n";

    // SplitMix64
    std::cout<<"SplitMix64(100)\n";
    SplitMix64 gen2(100);
    for(int i=0;i<10;i++) std::cout<<std::hex<<gen2.next()<<"\n";
    std::cout<<gen2.index_from_value(0x3ee5789041c98ac2)<<"\n";
    std::cout<<"\n";

    // xoshiro256**
    std::cout<<"xoshiro256ss(100)\n";
    xoshiro256ss gen3(100);
    for(int i=0;i<4;i++) std::cout<<std::hex<<gen3.next()<<"\n";
    std::cout<<"\n";

    return 0;
}
