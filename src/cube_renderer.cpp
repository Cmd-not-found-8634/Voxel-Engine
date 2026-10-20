#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <utils/stb_image.h>
#include <utils/json.hpp>
#include <utils/jsonparse.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <array>

#include <shader/shader.hpp>
#include <camera/camera.hpp>
#include <texture/texture.cpp>
#include <world/block.hpp>
#include <shader/parse_shader.hpp>
#include <world/terrain_generator_test.hpp>
#include <world/mesh_builder.hpp>

float lastFrame = 0.0f;

Camera camera;

/* ================= CALLBACK ================= */

void mouse_callback(GLFWwindow*,double x,double y){
    camera.mouse(x,y);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height){
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

int main(){

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

    /* GL_BLEND handles alpha channel (necessary for transparent blocks)*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* SHADERS */
    const char* vs = parse_shader("include/shader/vertex_shader.glsl");
    const char* fs = parse_shader("include/shader/fragment_shader.glsl");

    Shader shader(vs,fs);

    Texture texture("textures/textures.png");

    const std::uint64_t worldSeed = 123456789ULL;

    terrain_generator_test terrain(worldSeed);

    int chunkX = -1, chunkZ = -1;

    auto blocks = terrain.generateChunk(chunkX, chunkZ);

    int chunk_volume = blocks.size();

    generate_block_list();

    std::vector<block_str> chunk;

    int S = 256;
    int H = 64;

    for(int i = 0 ; i < chunk_volume ; i++){
        float x = static_cast<float>(chunkX*S + i/(H*S));
        float y = static_cast<float>((i/S)%H);
        float z = static_cast<float>(chunkZ*S + i%S);
        std::array<float,3> pos({x, y, z});

        int block_id = static_cast<int>(blocks[i]);

        chunk.push_back(block_str(pos, block_id));
    }

    MeshBuilder mesh_builder(chunk, S, H);

    std::cout<<"vertices = "<<mesh_builder.vertices.size()<<'\n';
    std::cout<<"indices = "<<mesh_builder.indices.size()<<'\n';
    std::cout<<"faces = "<<mesh_builder.cnt<<'\n';

    while(!glfwWindowShouldClose(window)){

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
        shader.setVec3("lightPos",glm::vec3(0,100,0));

        texture.bind();

        mesh_builder.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}