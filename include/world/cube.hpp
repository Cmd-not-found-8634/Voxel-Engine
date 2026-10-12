#pragma once

#include <vector>
#include <array>
#include <map>
#include <string>

#include <utils/jsonparse.hpp>
#include <world/geometry.hpp>

nlohmann::json blocks = parse_json("include/world/blocks.json");
nlohmann::json minecraft_tiles = parse_json("include/world/minecraft_tiles.json");

class Block
{

public:

Geometry geometry;
bool solid, transparent;
std::array<float,3> position;

Block(const std::array<float,3>& pos, const std::string& _block){
    position = pos;
    nlohmann::json data = blocks[_block];
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
}

};
