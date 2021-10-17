#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include <iostream>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);


// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//defines for camera
float radius = 10.0f;
float camX = 0; // sin(glfwGetTime())* radius;
float camY = 0; // 
float camZ = -2; // cos(glfwGetTime())* radius;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("shaderfiles/7.3.camera.vs", "shaderfiles/7.3.camera.fs");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {

        -4.0f, -4.0f, -4.0f, 0.0f, 0.0f, //PLANE bottom left /////  TILED TEXTURE ////
         4.0f, -4.0f, -4.0f, 6.0f, 0.0f, // bottom right
         4.0f, -4.0f, 4.0f,  6.0f, 4.0f, // bottom rght
         4.0f, -4.0f, 4.0f,  6.0f, 4.0f, // top right
        -4.0f, -4.0f, 4.0f,  0.0f, 4.0f, // top left
        -4.0f, -4.0f, -4.0f, 0.0f, 0.0f // bottom right
    };

    float mirrorVertices[] = {
        // positions          // normals           // texture coords
        -0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //MIRROR PLANE bottom /////  TILED TEXTURE ////
         0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // 
         0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // 
         0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, //
        -0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // 
        -0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // 

        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //MIRROR PLANE top /////  TILED TEXTURE //// #5
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
        -0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5 BACK
        -0.3f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8

        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5 LEFT
        -0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
        -0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4
        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

         0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2 RIGHT
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
         0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #3
         0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7

        -0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4 TOP
         0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #3
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -0.3f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4
         0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -0.3f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

        -0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1 BOTTOM
         0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
        -0.3f, -2.0f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         0.3f, -1.9f,  3.2f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f,  // #8




        -1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //MIRROR PLANE top /////  TILED TEXTURE //// #4
         1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #3
         1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
         1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
        -1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4

        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //MIRROR PLANE top /////  TILED TEXTURE //// #5
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
        -1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5 BACK
        -1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8

        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5 LEFT
        -1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
        -1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4
        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

         1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2 RIGHT
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
         1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #3
         1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7

        -1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4 TOP
         1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #3
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -1.7f, -2.0f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #4
         1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f, // #7
        -1.7f, -1.9f, -1.7f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // #5

        -1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1 BOTTOM
         1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #2
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f, // #8
        -1.7f, -2.0f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #1
        -1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f, // #6
         1.7f, -1.9f,  1.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f  // #8
    };


    //pyramid + cube
    float topVertices[] = {
        //pyramid
         0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,  //1 TOP
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,  //1 BOTTOM FRONT LEFT
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  //1 BOTTOM FRONT RIGHT

         0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,  //2 TOP
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  //2 BOTTOM FRONT RIGHT
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,  //2 BOTTOM BACK RIGHT

         0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,  //3 TOP
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  //3 BOTTOM BACK RIGHT
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,  //3 BOTTOM BACK LEFT

         0.0f,  1.0f,  0.0f,  0.0f,  0.0f, -1.0f,  0.5f,  1.0f,  //4 TOP
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  //4 BOTTOM BACK LEFT
        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,  //4 BOTTOM FRONT LEFT

        -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,  //5 BOTTOM FRONT LEFT   1
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,   //5 BOTTOM FRONT RIGHT  2
        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  //5 BOTTOM BACK LEFT   4

        -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,  //6 BOTTOM BACK LEFT  4
         0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,  //6 BOTTOM BACK RIGHT  3
         0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f  //6 BOTTOM FRONT RIGHT  2
    };
    float bottomVertices[] = {
        //cube
       -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //T1
        0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
       -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

       -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //t2
        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,

       -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //f1
        0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,

       -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //f2
       -0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,

        0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //R1
        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,

        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //R2
        0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f, //B1
        0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
       -0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

       -0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f, //B2
       -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,

       -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //L1
       -0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
       -0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,

       -0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f, //L2
       -0.5f,  0.0f,  0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
       -0.5f,  0.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

       -0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //D1
        0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,

        0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f, //D2
       -0.5f, -1.0f,  0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
       -0.5f, -1.0f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f

    };

    // position for plane
    glm::vec3 position[] = {
        glm::vec3(0.0f,  0.0f,  0.0f)
    };

    // configure all VAOs and VBOs
        //plane VAO and VBO
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    //mirror VAO and VBO
    unsigned int mirrorVBO, mirrorVAO;
    glGenVertexArrays(1, &mirrorVAO);
    glGenBuffers(1, &mirrorVBO);

    glBindBuffer(GL_ARRAY_BUFFER, mirrorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(mirrorVertices), mirrorVertices, GL_STATIC_DRAW);

    glBindVertexArray(mirrorVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // light's VAO (VBO stays the same as plane's VBO; the vertices are the same for the light object which is also a plane)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

   

    //pyramind + cube
    unsigned int topVBO, topVAO; //pyramid
    glGenVertexArrays(1, &topVAO);
    glGenBuffers(1, &topVBO);

    glBindBuffer(GL_ARRAY_BUFFER, topVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(topVertices), topVertices, GL_STATIC_DRAW);

    glBindVertexArray(topVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    unsigned int bottomVBO, bottomVAO; //cube
    glGenVertexArrays(1, &bottomVAO);
    glGenBuffers(1, &bottomVBO);

    glBindBuffer(GL_ARRAY_BUFFER, bottomVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(bottomVertices), bottomVertices, GL_STATIC_DRAW);

    glBindVertexArray(bottomVAO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // load and create a texture 
    // -------------------------
    unsigned int texture1, texture2, texture3, texture4;

    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data = stbi_load("images/blonde.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("images/chevron.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 3
    // ---------
    glGenTextures(1, &texture3);
    glBindTexture(GL_TEXTURE_2D, texture3);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load("images/marble2.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 4
    // ---------
    glGenTextures(1, &texture4);
    glBindTexture(GL_TEXTURE_2D, texture4);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    unsigned char* data2 = stbi_load("images/bowlinner.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data2);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use();
    ourShader.setInt("texture1", 0);
    ourShader.setInt("texture2", 1);
    ourShader.setInt("texture3", 2);
    ourShader.setInt("texture4", 3);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //bind textures on corresponding texture units

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);



        // activate shader
        ourShader.use();

        // camera/view transformation
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        ourShader.setMat4("view", view);


        // world transformation
        glm::mat4 model = glm::mat4(1.0f);
        ourShader.setMat4("model", model);

       
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // draw floor
        glBindVertexArray(VAO);
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // bind specular map
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);

        // draw mirror
        glBindVertexArray(mirrorVAO);
        float angle = 20;
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(1.0f, -1.99f, -1.0f));
        model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, -1.0f, 0.0f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 72);

        //pyramid top
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture2);
        glBindVertexArray(topVAO);
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(1.0f, -2.84f, -1.0f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 18);

        //cube bottom
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture3);
        glBindVertexArray(bottomVAO);
        model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
        model = glm::translate(model, glm::vec3(1.0f, -2.84f, -1.0f));
        ourShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);

    }
    // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
    // -------------------------------------------------------------------------------
    glfwSwapBuffers(window);
    glfwPollEvents();


    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    float moveSpeed = 3.5f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        cameraPos += cameraFront * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        cameraPos -= cameraFront * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    {
        cameraPos += cameraUp * moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    {
        cameraPos -= cameraUp * moveSpeed;
    }




}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}