#pragma once

#include <vector>
#include <array>
#include <map>
#include <string>

#include <utils/jsonparse.hpp>
#include <world/geometry.hpp>

nlohmann::json blocks = parse_json("include/world/blocks.json");
nlohmann::json minecraft_tiles = parse_json("include/world/minecraft_tiles.json");

class Block{

public:
int id;
Geometry geometry;
bool solid, transparent, render;

Block(const std::string& _block){
    nlohmann::json data = blocks[_block];
    id = data["id"];
    nlohmann::json texture = data["textures"];

    geometry.set_geometry(data["geometry"]);

    for(const auto& [face, tile] : texture.items()){
        nlohmann::json pos = minecraft_tiles[tile];
        float x_0 = pos["x0"];
        float y_0 = pos["y0"];
        float x_1 = pos["x1"];
        float y_1 = pos["y1"];
        geometry.set_abs_texcoords(face, x_0, y_0, x_1, y_1);
    }

    solid = data["properties"]["solid"];
    transparent = data["properties"]["transparent"];
    render = data["properties"]["render"];
}

};

std::vector<Block> block_list;

void generate_block_list(){
    block_list.clear();
    block_list.reserve(blocks.size());
    for(const auto& [block, _] : blocks.items()){
        block_list.emplace_back(block);
    }
    std::sort(block_list.begin(), block_list.end(), [](const auto& a, const auto& b) { return a.id < b.id; });
}

Block& get_block(int id){
    return block_list[id];
}

class block_str{
public:
    const std::array<float, 3> position;
    const int id;
    block_str(const std::array<float, 3>& pos, int b_id): position(pos), id(b_id) {}
};
