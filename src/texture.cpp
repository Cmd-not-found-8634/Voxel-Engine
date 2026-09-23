#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <iostream>

/* ================= TEXTURE CLASS ================= */

class Texture
{

public:

unsigned int ID;

Texture(const char* path){
    glGenTextures(1,&ID);
    glBindTexture(GL_TEXTURE_2D,ID);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);

    int w,h,c;

    //stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path,&w,&h,&c,0);

    if (!data){
        std::cout << "FAILED TO LOAD TEXTURE: " << path << std::endl;
        std::cout << "STB ERROR: " << stbi_failure_reason() << std::endl;
        return;
    }

    GLenum format;

    if (c == 1) format = GL_RED;
    else if (c == 3) format = GL_RGB;
    else if (c == 4) format = GL_RGBA;
    else {
        std::cout << "Unsupported channel count: " << c << std::endl;
        stbi_image_free(data);
        return;
    }

    glTexImage2D( GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data );

    // Makes the pixelated images blurry
    // glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
}

void bind(){
    glBindTexture(GL_TEXTURE_2D,ID);
}

};