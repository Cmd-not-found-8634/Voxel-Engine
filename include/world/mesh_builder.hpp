#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector>

#include <world/cube.hpp>

class MeshBuilder
{

std::array<int,3> index_2_position(int idx, int S, int H){
    return std::array<int,3>({idx/S/H, (idx/S)%H, idx%S});
}

int position_2_index(const std::array<int,3>& pos, int S, int H){
    return (pos[0]*H+pos[1])*S+pos[2];
}

bool is_overlap(const std::vector<Block>& chunk, int blk1, int n1, int blk2, int n2){
    bool overlap = true;
    std::array<float,3> norm1 = chunk[blk1].geometry.normals[n1];
    for(int i = 0 ; i < 4 ; i++){
        std::array<float,3> pnt1 = chunk[blk1].geometry.points[n1][i];
        std::array<float,3> pnt2 = chunk[blk2].geometry.points[n2][i];
        if(pnt1[0]+norm1[0]!=pnt2[0] || pnt1[1]+norm1[1]!=pnt2[1] || pnt1[2]+norm1[2]!=pnt2[2]) overlap = false;
    }
    return overlap;
}

public:

unsigned int VAO,VBO,EBO;

int cnt = 0;

std::vector<float> vertices;
std::vector<unsigned int> indices;

void build_mesh(std::vector<Block>& chunk, int S, int H){

    int chunk_volume = static_cast<int>(chunk.size());

    for(int i = 0 ; i < chunk_volume ; i++){
        const Block& blk = chunk[i];

        std::array<int,3> pos = index_2_position(i, S, H);

        const Geometry& gmt = blk.geometry;

        for(int j = 0 ; j < gmt.count_faces ; j++){
            std::array<float,3> normal = gmt.normals[j];

            int nx = pos[0] + static_cast<int>(normal[0]);
            int ny = pos[1] + static_cast<int>(normal[1]);
            int nz = pos[2] + static_cast<int>(normal[2]);

            if(nx >= 0 && nx < S && ny >= 0 && ny < H && nz >= 0 && nz < S ){
                    int adj_idx = position_2_index({nx, ny, nz}, S, H);

                    //!chunk[adj_idx].transparent && is_overlap(chunk, i, j, adj_idx, j^1)
                    if(!chunk[adj_idx].transparent) continue;
            }

            for(int k = 0; k < 4; k++){
                const auto& p = gmt.points[j][k];

                // Position
                vertices.push_back(p[0] + blk.position[0]);
                vertices.push_back(p[1] + blk.position[1]);
                vertices.push_back(p[2] + blk.position[2]);

                // Normal
                vertices.insert(vertices.end(), gmt.normals[j].begin(), gmt.normals[j].end());

                // UV
                vertices.insert(vertices.end(), gmt.texcoords[j][k].begin(), gmt.texcoords[j][k].end());
            }

            for(unsigned int index : gmt.EBO[j]) indices.push_back(index + 4 * cnt);
            
            ++cnt;
        }
    }
}

MeshBuilder(std::vector<Block>& chunk, int S, int H){
    build_mesh(chunk, S, H);

    glGenVertexArrays(1,&VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1,&VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,vertices.size()*sizeof(float),vertices.data(),GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW); 

    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void draw(){
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES,static_cast<GLsizei>(indices.size()),GL_UNSIGNED_INT,nullptr);
    glBindVertexArray(0);
}

};
