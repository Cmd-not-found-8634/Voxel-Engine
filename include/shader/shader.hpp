#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>

/* ================= SHADER CLASS ================= */

class Shader
{
public:

unsigned int ID;

Shader(const char* vs, const char* fs){

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex,1,&vs,NULL);
    glCompileShader(vertex);

    // DEBUG
    int success;
    char infoLog[512];

    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);

    if (!success){
        glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
        std::cerr<<"VERTEX SHADER ERROR:\n"<<infoLog<<'\n';
    }

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment,1,&fs,NULL);
    glCompileShader(fragment);

    // DEBUG
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);

    if (!success){
        glGetShaderInfoLog(fragment,512,nullptr,infoLog);
        std::cerr<<"FRAGMENT SHADER ERROR:\n"<<infoLog<<'\n';
    }

    ID = glCreateProgram();

    glAttachShader(ID,vertex);
    glAttachShader(ID,fragment);
    glLinkProgram(ID);

    glDeleteShader(vertex);
    glDeleteShader(fragment);

}

void use(){
    glUseProgram(ID);
}

void setMat4(const std::string &name, glm::mat4 mat){
    glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(mat));
}

void setVec3(const std::string &name, glm::vec3 v){
    glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,&v[0]);
}

};