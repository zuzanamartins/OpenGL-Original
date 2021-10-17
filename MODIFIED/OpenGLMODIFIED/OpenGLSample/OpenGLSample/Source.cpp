#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "cylinder.h"
#include "camera.h"

#include <iostream>

//callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
unsigned int loadTexture(const char* path);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, -2.0f, 8.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

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
	//set callbacks
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

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
	Shader lightingShader("shaderfiles/6.multiple_lights.vs", "shaderfiles/6.multiple_lights.fs");
	Shader lightCubeShader("shaderfiles/6.light_cube.vs", "shaderfiles/6.light_cube.fs");

	// set up vertex data (and buffer(s)) and configure vertex attributes
	// ------------------------------------------------------------------
	float vertices[] = {
		// positions          // normals           // texture coords
		-4.0f, -4.0f, -4.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f, //PLANE   TILED TEXTURE ////
		 4.0f, -4.0f, -4.0f, 0.0f,  0.0f, -1.0f, 6.0f, 0.0f,
		 4.0f, -4.0f,  4.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f,
		 4.0f, -4.0f,  4.0f, 0.0f,  0.0f, -1.0f, 6.0f, 4.0f,
		-4.0f, -4.0f,  4.0f, 0.0f,  0.0f, -1.0f, 0.0f, 4.0f,
		-4.0f, -4.0f, -4.0f, 0.0f,  0.0f, -1.0f, 0.0f, 0.0f
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
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(1.0f,  2.0f,  -3.0f),
		glm::vec3(-4.3f,  2.0f,  -3.0f)
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

	//2 cylinders
	unsigned int VBO2, VAO2;
	glGenVertexArrays(1, &VAO2);
	glGenBuffers(1, &VBO2);
	glBindVertexArray(VAO2);
	glBindBuffer(GL_ARRAY_BUFFER, VBO2);

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


	// load textures (we now use a utility function to keep the code more organized)
	// -----------------------------------------------------------------------------
	unsigned int diffuseMap = loadTexture("images/blonde.jpg");
	unsigned int specularMap = loadTexture("images/blondeBW.jpg");
	unsigned int bowlMap = loadTexture("images/marble2.jpg");
	unsigned int innerMap = loadTexture("images/bowlinner.jpg");
	unsigned int anotherMap = loadTexture("images/newbrick.jpg");
	unsigned int blackMap = loadTexture("images/blackflowers.jpg");
	unsigned int whiteMap = loadTexture("images/whitebrick.jpg");

	// shader configuration
	// --------------------
	lightingShader.use();
	lightingShader.setInt("material.diffuseMap", 0);
	lightingShader.setInt("material.specularMap", 1);
	lightingShader.setInt("material.bowlMap", 2);
	lightingShader.setInt("material.innerMap", 3);
	lightingShader.setInt("material.anotherMap", 4);
	lightingShader.setInt("material.blackMap", 5);
	lightingShader.setInt("material.whiteMap", 6);


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
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// be sure to activate shader when setting uniforms/drawing objects
		lightingShader.use();
		lightingShader.setVec3("viewPos", camera.Position);
		lightingShader.setFloat("material.shininess", 32.0f);

		//colors for lights
		glm::vec3 pointLightColors[] = {
		glm::vec3(1.0f, 1.0f, 1.0f), //WHITE
		glm::vec3(0.6f, 0.0f, 0.8f)  //PURPLE
		};
		//uniforms for lights, #1 is white point light, #2 is purple point pink, spot light is also white
		// point light 1
		lightingShader.setVec3("pointLights[0].position", pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z);
		lightingShader.setVec3("pointLights[0].ambient", pointLightColors[0].x * 0.8, pointLightColors[0].y * 0.8, pointLightColors[0].z * 0.8);
		lightingShader.setVec3("pointLights[0].diffuse", pointLightColors[0].x * 0.8, pointLightColors[0].y * 0.8, pointLightColors[0].z * 0.8);
		lightingShader.setVec3("pointLights[0].specular", pointLightColors[0].x * 0.8, pointLightColors[0].y * 0.8, pointLightColors[0].z * 0.8);
		lightingShader.setFloat("pointLights[0].constant", 0.0f);
		lightingShader.setFloat("pointLights[0].linear", 0.09);
		lightingShader.setFloat("pointLights[0].quadratic", 0.032);
		// point light 2
		lightingShader.setVec3("pointLights[1].position", pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z);
		lightingShader.setVec3("pointLights[1].ambient", pointLightColors[1].x * 0.5, pointLightColors[1].y * 0.5, pointLightColors[1].z * 0.5);
		lightingShader.setVec3("pointLights[1].diffuse", pointLightColors[1].x * 0.5, pointLightColors[1].y * 0.5, pointLightColors[1].z * 0.5);
		lightingShader.setVec3("pointLights[1].specular", pointLightColors[1].x * 0.5, pointLightColors[1].y * 0.5, pointLightColors[1].z * 0.5);
		lightingShader.setFloat("pointLights[1].constant", 1.0f);
		lightingShader.setFloat("pointLights[1].linear", 0.09);
		lightingShader.setFloat("pointLights[1].quadratic", 0.032);
		// point light 3
		lightingShader.setVec3("pointLights[2].position", pointLightPositions[2]);
		lightingShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[2].constant", 1.0f);
		lightingShader.setFloat("pointLights[2].linear", 0.09);
		lightingShader.setFloat("pointLights[2].quadratic", 0.032);
		// point light 4
		lightingShader.setVec3("pointLights[3].position", pointLightPositions[3]);
		lightingShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		lightingShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		lightingShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		lightingShader.setFloat("pointLights[3].constant", 1.0f);
		lightingShader.setFloat("pointLights[3].linear", 0.09);
		lightingShader.setFloat("pointLights[3].quadratic", 0.032);
		// spotLight
		lightingShader.setVec3("spotLight.position", camera.Position);
		lightingShader.setVec3("spotLight.direction", camera.Front);
		lightingShader.setVec3("spotLight.ambient", pointLightColors[0].x * 0.7, pointLightColors[0].y * 0.7, pointLightColors[0].z * 0.7);
		lightingShader.setVec3("spotLight.diffuse", pointLightColors[0].x * 0.7, pointLightColors[0].y * 0.7, pointLightColors[0].z * 0.7);
		lightingShader.setVec3("spotLight.specular", pointLightColors[0].x * 0.7, pointLightColors[0].y * 0.7, pointLightColors[0].z * 0.7);
		lightingShader.setFloat("spotLight.constant", 1.0f);
		lightingShader.setFloat("spotLight.linear", 0.09);
		lightingShader.setFloat("spotLight.quadratic", 0.032);
		lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(20.0f)));


		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setMat4("projection", projection);
		lightingShader.setMat4("view", view);

		// world transformation
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setMat4("model", model);

		// bind diffuse map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);
		// bind specular map
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		// draw floor
		glBindVertexArray(VAO);
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// bind specular map
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, blackMap);

		// draw mirror
		glBindVertexArray(mirrorVAO);
		float angle = 20;
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, -1.99f, -1.0f));
		model = glm::rotate(model, glm::radians(angle), glm::vec3(0.0f, -1.0f, 0.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 72);

		//pyramid top
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, anotherMap);
		glBindVertexArray(topVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, -2.84f, -1.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 18);

		//cube bottom
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, whiteMap);
		glBindVertexArray(bottomVAO);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(1.0f, -2.84f, -1.0f));
		lightingShader.setMat4("model", model);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		//cylinders

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, bowlMap);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-2.0f, -3.79f, 1.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C(1.1, 30, 0.4, true, true, true);
		C.render();


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, innerMap);
		glBindVertexArray(VAO2);
		model = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first		
		model = glm::translate(model, glm::vec3(-2.0f, -3.79f, 1.0f));
		lightingShader.setMat4("model", model);

		static_meshes_3D::Cylinder C2(0.9, 30, 0.41, true, true, true);
		C2.render();



		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// optional: de-allocate all resources once they've outlived their purpose:
	// ------------------------------------------------------------------------
	glDeleteVertexArrays(1, &VAO);
	glDeleteVertexArrays(1, &VAO2);
	glDeleteVertexArrays(1, &topVAO);
	glDeleteVertexArrays(1, &bottomVAO);
	glDeleteVertexArrays(1, &mirrorVAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &VBO2);
	glDeleteBuffers(1, &topVBO);
	glDeleteBuffers(1, &bottomVBO);
	glDeleteBuffers(1, &mirrorVBO);

	// glfw: terminate, clearing all previously allocated GLFW resources.
	// ------------------------------------------------------------------
	glfwTerminate();
	return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) //forward
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)  //backward
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)  //left
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)  //right
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)  //upward
		camera.ProcessKeyboard(UPWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)   //downward
		camera.ProcessKeyboard(DOWNWARD, deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}