#pragma once

#include <vector>
#include <array>
#include <map>
#include <string>

#include <jsonparse.hpp>

nlohmann::json geometry = parse_json("textures/geometry.json");

class Geometry
{

public:

int count_faces;
std::map<std::string,int> mp;
std::vector<std::array<float,3>> normals;
std::vector<std::array<std::array<float,3>,4>> points;
std::vector<std::array<std::array<float,2>,4>> texcoords;
std::vector<std::array<unsigned int,6>> EBO;

void set_geometry(const std::string& _geometry){
    mp.clear();
    normals.clear();
    points.clear();
    texcoords.clear();
    EBO.clear();

    nlohmann::json data = geometry[_geometry];

    std::vector<std::string> faces = data["faces"];

    count_faces = static_cast<int>(faces.size());

    for(int i=0;i<count_faces;i++){
        mp[faces[i]]=i;

        normals.push_back(data[faces[i]]["normals"]);

        points.push_back(data[faces[i]]["points"]);
        texcoords.push_back(data[faces[i]]["texcoords"]);

        EBO.push_back(data[faces[i]]["EBO"]);
    }
}

Geometry(const std::string& _geometry = "cube"){
    set_geometry(_geometry);
}

void set_abs_texcoords(const std::string& face, float x_0, float y_0, float x_1, float y_1){
    auto ptr = mp.find(face);

    if(ptr==mp.end()) return;

    int i=ptr->second;

    for(std::array<float,2>& j: texcoords[i]){
        j[0] = x_0 + j[0]*(x_1-x_0);
        j[1] = y_0 + j[1]*(y_1-y_0);
    }
}

};
