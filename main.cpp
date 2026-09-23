#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <fstream>
#include <json.hpp>
#include <string>
#include <vector>

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

nlohmann::json blocks, minecraft_tiles;

nlohmann::json parse_json(const std::string& file_path){
    std::ifstream file(file_path);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: "+file_path);
    }
    nlohmann::json data;
    file>>data;
    return data;
}

/* ================= SHADER CLASS ================= */

class Shader
{
public:

unsigned int ID;

Shader(const char* vs,const char* fs)
{

unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
glShaderSource(vertex,1,&vs,NULL);
glCompileShader(vertex);

unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
glShaderSource(fragment,1,&fs,NULL);
glCompileShader(fragment);

ID = glCreateProgram();

glAttachShader(ID,vertex);
glAttachShader(ID,fragment);
glLinkProgram(ID);

glDeleteShader(vertex);
glDeleteShader(fragment);

}

void use()
{
glUseProgram(ID);
}

void setMat4(const std::string &name, glm::mat4 mat)
{
glUniformMatrix4fv(glGetUniformLocation(ID,name.c_str()),1,GL_FALSE,glm::value_ptr(mat));
}

void setVec3(const std::string &name, glm::vec3 v)
{
glUniform3fv(glGetUniformLocation(ID,name.c_str()),1,&v[0]);
}

};

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

Camera()
{

Position = glm::vec3(0.0f,0.0f,5.0f);
Front = glm::vec3(0.0f,0.0f,-1.0f);
Up = glm::vec3(0.0f,1.0f,0.0f);

yaw = -90.0f;
pitch = 0.0f;

lastX = SCR_WIDTH/2;
lastY = SCR_HEIGHT/2;

}

glm::mat4 getView()
{
return glm::lookAt(Position,Position+Front,Up);
}

void keyboard(GLFWwindow* window)
{

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

void mouse(double xpos,double ypos)
{

if(firstMouse)
{
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

Camera camera;

/* ================= TEXTURE CLASS ================= */

class Texture
{

public:

unsigned int ID;

Texture(const char* path)
{

glGenTextures(1,&ID);
glBindTexture(GL_TEXTURE_2D,ID);

glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);

glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

int w,h,c;

unsigned char* data = stbi_load(path,&w,&h,&c,0);

glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,w,h,0,GL_RGB,GL_UNSIGNED_BYTE,data);

glGenerateMipmap(GL_TEXTURE_2D);

stbi_image_free(data);

}

void bind()
{
glBindTexture(GL_TEXTURE_2D,ID);
}

};

struct Vertices{

};

struct Normals{

};

struct UVRect{

};

struct Block{

};

/* ================= CUBE CLASS ================= */

class Cube
{

public:

unsigned int VAO,VBO;

Cube()
{

float vertices[] = {
 // positions        // normals         // texcoords

 // back face
 -0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.67f,0.5f,
  0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,0.5f,
  0.5f, 0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,
  0.5f, 0.5f,-0.5f, 0.0f,0.0f,-1.0f, 1.0f,0.0f,
 -0.5f, 0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.67f,0.0f,
 -0.5f,-0.5f,-0.5f, 0.0f,0.0f,-1.0f, 0.67f,0.5f,

 // front face
 -0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 0.67f,0.5f,
  0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,0.5f,
  0.5f, 0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,0.0f,
  0.5f, 0.5f,0.5f, 0.0f,0.0f,1.0f, 1.0f,0.0f,
 -0.5f, 0.5f,0.5f, 0.0f,0.0f,1.0f, 0.67f,0.0f,
 -0.5f,-0.5f,0.5f, 0.0f,0.0f,1.0f, 0.67f,0.5f,

 // left face
 -0.5f, 0.5f, 0.5f,-1.0f,0.0f,0.0f, 1.0f,0.0f,
 -0.5f, 0.5f,-0.5f,-1.0f,0.0f,0.0f, 0.67f,0.0f,
 -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f, 0.67f,0.5f,
 -0.5f,-0.5f,-0.5f,-1.0f,0.0f,0.0f, 0.67f,0.5f,
 -0.5f,-0.5f, 0.5f,-1.0f,0.0f,0.0f, 1.0f,0.5f,
 -0.5f, 0.5f, 0.5f,-1.0f,0.0f,0.0f, 1.0f,0.0f,

 // right face
 0.5f, 0.5f, 0.5f,1.0f,0.0f,0.0f, 1.0f,0.0f,
 0.5f, 0.5f,-0.5f,1.0f,0.0f,0.0f, 0.67f,0.0f,
 0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f, 0.67f,0.5f,
 0.5f,-0.5f,-0.5f,1.0f,0.0f,0.0f, 0.67f,0.5f,
 0.5f,-0.5f, 0.5f,1.0f,0.0f,0.0f, 1.0f,0.5f,
 0.5f, 0.5f, 0.5f,1.0f,0.0f,0.0f, 1.0f,0.0f,

 // bottom
 -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f, 0.33f,0.5f,
 0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f, 0.67f,0.5f,
 0.5f,-0.5f, 0.5f,0.0f,-1.0f,0.0f, 0.67f,0.0f,
 0.5f,-0.5f, 0.5f,0.0f,-1.0f,0.0f, 0.67f,0.0f,
 -0.5f,-0.5f, 0.5f,0.0f,-1.0f,0.0f, 0.33f,0.0f,
 -0.5f,-0.5f,-0.5f,0.0f,-1.0f,0.0f, 0.33f,0.5f,

 // top
 -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f, 0.0f,0.5f,
 0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f, 0.33f,0.5f,
 0.5f,0.5f,0.5f,0.0f,1.0f,0.0f, 0.33f,0.0f,
 0.5f,0.5f,0.5f,0.0f,1.0f,0.0f, 0.33f,0.0f,
 -0.5f,0.5f,0.5f,0.0f,1.0f,0.0f, 0.0f,0.0f,
 -0.5f,0.5f,-0.5f,0.0f,1.0f,0.0f, 0.0f,0.5f
};

glGenBuffers(1,&VBO);
glBindBuffer(GL_ARRAY_BUFFER,VBO);
glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

glGenVertexArrays(1,&VAO);
glBindVertexArray(VAO);

glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
glEnableVertexAttribArray(0);

glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(3*sizeof(float)));
glEnableVertexAttribArray(1);

glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(6*sizeof(float)));
glEnableVertexAttribArray(2);

}

void draw()
{

glBindVertexArray(VAO);
glDrawArrays(GL_TRIANGLES,0,36);

}

};

/* ================= CALLBACK ================= */

void mouse_callback(GLFWwindow*,double x,double y)
{
camera.mouse(x,y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height); // Adjust OpenGL viewport
}

class FPSCounter
{
public:

double lastTime = 0.0;
int frames = 0;

void update(GLFWwindow* window)
{
    double currentTime = glfwGetTime();
    frames++;

    if(currentTime - lastTime >= 1.0)
    {
        std::string title =
            "OpenGL Scene | FPS: " + std::to_string(frames);

        glfwSetWindowTitle(window,title.c_str());

        frames = 0;
        lastTime = currentTime;
    }
}

};

// Contains Initialisation logic of the complete program
bool init(){
    if(!glfwInit()){
        std::cout << "Failed to Initialise GLFW" <<std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
    return true;
}

// window creation and customization code
GLFWwindow* createWindow(int width,int height,const char* title,bool resizable){
    glfwWindowHint(GLFW_RESIZABLE, resizable?GLFW_TRUE:GLFW_FALSE);
    GLFWwindow* window = glfwCreateWindow(width,height,title,NULL,NULL);
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        return NULL;
    }
    return window;
}

// setting up callbacks and other helpful utils
void setupWindowCallbacks(GLFWwindow* window){
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
}

/* ================= MAIN ================= */

int main()
{

minecraft_tiles = parse_json("textures/minecraft_tiles.json");
blocks = parse_json("textures/blocks.json");

init();

GLFWwindow* window = createWindow(SCR_WIDTH,SCR_HEIGHT,"OpenGL OOP Scene",true);

if (window == NULL){
    glfwTerminate();
    return -1;
}

glfwMakeContextCurrent(window);

FPSCounter fps;

setupWindowCallbacks(window);

gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

glEnable(GL_DEPTH_TEST);

/* SHADERS */

const char* vs=
"#version 330 core\n"
"layout(location=0) in vec3 aPos;"
"layout(location=1) in vec3 aNormal;"
"layout(location=2) in vec2 aTex;"
"out vec3 FragPos;"
"out vec3 Normal;"
"out vec2 TexCoord;"
"uniform mat4 model;"
"uniform mat4 view;"
"uniform mat4 projection;"
"void main(){"
"FragPos=vec3(model*vec4(aPos,1));"
"Normal=mat3(transpose(inverse(model)))*aNormal;"
"TexCoord=aTex;"
"gl_Position=projection*view*vec4(FragPos,1);"
"}";

const char* fs=
"#version 330 core\n"
"out vec4 FragColor;"
"in vec3 FragPos;"
"in vec3 Normal;"
"in vec2 TexCoord;"
"uniform vec3 viewPos;"
"uniform vec3 lightPos;"
"uniform sampler2D texture1;"
"void main(){"
"vec3 lightColor=vec3(1);"
"float ambientStrength=0.2;"
"vec3 ambient=ambientStrength*lightColor;"
"vec3 norm=normalize(Normal);"
"vec3 lightDir=normalize(lightPos-FragPos);"
"float diff=max(dot(norm,lightDir),0);"
"vec3 diffuse=diff*lightColor;"
"vec3 viewDir=normalize(viewPos-FragPos);"
"vec3 reflectDir=reflect(-lightDir,norm);"
"float spec=pow(max(dot(viewDir,reflectDir),0),32);"
"vec3 specular=0.5*spec*lightColor;"
"vec3 result=(ambient+diffuse+specular)*texture(texture1,TexCoord).rgb;"
"FragColor=vec4(result,1);"
"}";

Shader shader(vs,fs);

Texture texture("textures/texture_map.png");

Cube cube;

glm::vec3 cubePositions[441];

for(int i=-10;i<=10;i++){
    for(int j=-10;j<=10;j++){
        cubePositions[(i+10)*21+(j+10)]=glm::vec3(i,-1,j);
    }
}

while(!glfwWindowShouldClose(window))
{

float currentFrame=glfwGetTime();
deltaTime=currentFrame-lastFrame;
lastFrame=currentFrame;

camera.keyboard(window);

fps.update(window);

glClearColor(0.1,0.1,0.1,1);
glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

shader.use();

glm::mat4 view=camera.getView();

glm::mat4 projection=glm::perspective(glm::radians(45.0f),(float)SCR_WIDTH/SCR_HEIGHT,0.1f,100.0f);

shader.setMat4("view",view);
shader.setMat4("projection",projection);

shader.setVec3("viewPos",camera.Position);
shader.setVec3("lightPos",glm::vec3(1.2,1,2));

texture.bind();

for(auto& pos : cubePositions)
{

glm::mat4 model=glm::mat4(1);

model=glm::translate(model,pos);

model=glm::rotate(model,0.0f,glm::vec3(1,0.3,0.5));

shader.setMat4("model",model);

cube.draw();

}

glfwSwapBuffers(window);
glfwPollEvents();
}

glfwTerminate();

return 0;

}