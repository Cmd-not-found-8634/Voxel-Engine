#pragma once

#include <noise/perlin_noise.hpp>
#include <cstdint>
#include <vector>
#include <iostream>

class terrain_generator_test{
private:
    int H=64;
    int S=256;
    int del=32;
    int sea_level=32;
    PerlinNoise pn;
public:
    terrain_generator_test(std::uint64_t seed): pn(seed){}
    std::vector<int> generateChunk(int chunkX, int chunkZ){
        std::vector<double> mp=generate_height_map(chunkX,chunkZ);
        std::vector<int> blocks(S*S*H);
        for(int i=0;i<S;i++){
            for(int j=0;j<H;j++){
                for(int k=0;k<S;k++){
                    int ht=static_cast<int>(mp[i*S+k]);
                    double n3d=get3dnoise(chunkX*S+i,j,chunkZ*S+k);
                    blocks[(i*H+j)*S+k]=get_block(ht,j,n3d);
                }
            }
        }
        return blocks;
    }
    int get_block(int ht, int y, double n3d){
        if(n3d<-0.2) return 0;
        else if(y>ht && y>sea_level) return 0;
        else if(y>ht) return 5;
        else if(y==ht) return 3;
        else if(y>ht-3) return 2;
        else return 1;
    }

    double get3dnoise(int x, int y, int z){
        return pn.noise3d(static_cast<double>(x)/64,static_cast<double>(y)/64,static_cast<double>(z)/64)+
               pn.noise3d(static_cast<double>(x)/32,static_cast<double>(y)/32,static_cast<double>(z)/32)*0.500+
               pn.noise3d(static_cast<double>(x)/16,static_cast<double>(y)/16,static_cast<double>(z)/16)*0.250+
               pn.noise3d(static_cast<double>(x)/8,static_cast<double>(y)/8,static_cast<double>(z)/8)*0.125;
    }

    std::vector<double> generate_height_map(int chunkX, int chunkZ){
        std::vector<double> map(S*S);
        for(int i=0;i<S;i++){
            for(int j=0;j<S;j++){
                map[i*S+j]=pn.noise2d(static_cast<double>(chunkX*S+i)/64,static_cast<double>(chunkZ*S+j)/64)*del+
                           pn.noise2d(static_cast<double>(chunkX*S+i)/32,static_cast<double>(chunkZ*S+j)/32)*del/2+
                           pn.noise2d(static_cast<double>(chunkX*S+i)/16,static_cast<double>(chunkZ*S+j)/16)*del/4+
                           pn.noise2d(static_cast<double>(chunkX*S+i)/8,static_cast<double>(chunkZ*S+j)/8)*del/8+ sea_level;
                //std::cout<<map[i*S+j]<<" ";
            }
            //std::cout<<"\n";
        }
        return map;
    }
};
