#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;

/* ================= CAMERA CLASS ================= */

class Camera
{

public:

glm::vec3 Position;
glm::vec3 Front;
glm::vec3 Up;

float yaw;
float pitch;

float lastX;
float lastY;

bool firstMouse=true;

Camera(){
    Position = glm::vec3(0.0f,0.0f,5.0f);
    Front = glm::vec3(0.0f,0.0f,-1.0f);
    Up = glm::vec3(0.0f,1.0f,0.0f);

    yaw = -90.0f;
    pitch = 0.0f;

    lastX = SCR_WIDTH/2;
    lastY = SCR_HEIGHT/2;
}

glm::mat4 getView(){
    return glm::lookAt(Position,Position+Front,Up);
}

void keyboard(GLFWwindow* window){
    float speed = 3.5f * deltaTime;

    if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS)
    Position += speed * Front;

    if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS)
    Position -= speed * Front;

    if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
    Position -= glm::normalize(glm::cross(Front,Up))*speed;

    if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS)
    Position += glm::normalize(glm::cross(Front,Up))*speed;
}

void mouse(double xpos,double ypos){
    if(firstMouse){
        lastX=xpos;
        lastY=ypos;
        firstMouse=false;
    }

    float xoffset=xpos-lastX;
    float yoffset=lastY-ypos;

    lastX=xpos;
    lastY=ypos;

    float sensitivity=0.1f;

    xoffset*=sensitivity;
    yoffset*=sensitivity;

    yaw+=xoffset;
    pitch+=yoffset;

    if(pitch>89) pitch=89;
    if(pitch<-89) pitch=-89;

    glm::vec3 dir;

    dir.x = cos(glm::radians(yaw))*cos(glm::radians(pitch));
    dir.y = sin(glm::radians(pitch));
    dir.z = sin(glm::radians(yaw))*cos(glm::radians(pitch));

    Front = glm::normalize(dir);
}

};