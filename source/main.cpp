#include <glad/glad.h>-
#include <GLFW/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "stb_image.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.h"
#include "Cube.h"
#include "Model.h"
#include "HelperFunctions.h";
#include "Light.h";
#include "DrawingFunctions.h"
#include "VaoCreation.h"
#include "Model.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window);

float opacityOfTexture(GLFWwindow* window, Shader shader);

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

void mouse_callback(GLFWwindow* window, double xpos, double ypos);

//screen coordinates
int SCR_WIDTH = 1000;
int SCR_HEIGHT = 800;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;

float hasNormalMap = 1.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 lightColor = COLOR_CYAN;

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;

bool isLightBlinn = true;


//light possition
glm::vec3 lightPosition(0.0f, 2.0f, -1.0f);

glm::vec3 pointLightPositions[] = {
glm::vec3(0.7f, 0.2f, 2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f, 2.0f, -12.0f),
glm::vec3(0.0f, 0.0f, -3.0f)
};

glm::vec3 cubePostions[] = {
	glm::vec3(-0.2f, 1.0f, 0.0),
	glm::vec3(3.0f, 0.0f, 3.0),
	glm::vec3(-1.0f, 0.0f, 2.0)
};

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//this code is to make context on the window current and to initialize glad
	glfwMakeContextCurrent(window);
	gladLoadGL();
	glEnable(GL_DEPTH_TEST);

	//enables gama correction that is build in opengl
	glEnable(GL_FRAMEBUFFER_SRGB);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int rows = 5;
	unsigned int colums = 5;
	unsigned int totalAmount = rows * colums;


	Shader mainObjShader("VertexShader/AdvancedLightning/AdvancedLightningVertex.glsl", "FragmentShader/AdvancedLightning/AdvancedLightningFragmnet.glsl", "main");

	Shader lightSourceShader("VertexShader/AdvancedLightning/LightSourceVertex.glsl", "FragmentShader/AdvancedLightning/LightSourceFragment.glsl", "light sourece");

	Shader shadowMapShader("VertexShader/AdvancedLightning/ShadowMapVertex.glsl", "FragmentShader/AdvancedLightning/ShadowMapFragement.glsl", "shadow map");
	
	Shader floorShader("VertexShader/FloorVertex.glsl", "FragmentShader/FloorFragment.glsl", "floor");

	Shader gBufferShader("VertexShader/AdvancedLightning/gBufferVertex.glsl", "FragmentShader/AdvancedLightning/gBufferFragment.glsl", "gBuffer");

	Shader finalShaderStage("VertexShader/AdvancedLightning/FinalVertex.glsl", "FragmentShader/AdvancedLightning/FinalFragment.glsl", "final shader");

	Model stormtrooper("Assets/Model/stormtrooper/stormtrooper.obj", totalAmount);
	
	stbi_set_flip_vertically_on_load(true);

	// plane VAO
	unsigned int planeVAO = createVAO(planeVertices, sizeof(planeVertices)/sizeof(float));

	//final frame buffer VAO
	unsigned int screenQuadVAO = createVAO(HDRframeBufferVertecies, sizeof(planeVertices) / sizeof(float), false, true);

	//VBO, EBO and VAO for the square that represents light position
	unsigned int lightVAO = createVAO(lightVertices,sizeof(lightVertices)/sizeof(float), false);

	//cube VAO
	unsigned int cubeVAO = createVAO(cubeVertices, sizeof(cubeVertices) / sizeof(float));

	//----------------
	// INSTANCED MODEL
	//----------------
	float currentHeight = 0.0f;

	std::vector<glm::mat4>modelMatrices;

	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < colums; j++)
		{
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(j * 3, 0.0f, currentHeight));
			modelMatrices.push_back(model);
		}

		currentHeight += i + 3;
	}

	const unsigned int NR_LIGHTS = 32;
	std::vector<glm::vec3> lightPositions;
	std::vector<glm::vec3> lightColors;
	srand(13);
	for (unsigned int i = 0; i < NR_LIGHTS; i++)
	{
		// calculate slightly random offsets
		float xPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		float yPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 4.0);
		float zPos = static_cast<float>(((rand() % 100) / 100.0) * 6.0 - 3.0);
		lightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// also calculate random color
		float rColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float gColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		float bColor = static_cast<float>(((rand() % 100) / 200.0f) + 0.5); // between 0.5 and 1.0
		lightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}


	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, (rows * colums) * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
	for(int i = 0; i<stormtrooper.meshes.size() ; i++)
	{
		unsigned int VAO = stormtrooper.meshes[i].VAO;

		glBindVertexArray(VAO);

		std::size_t v4s = sizeof(glm::vec4);

		// 1st colum of the matrix
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)0);

		//2 nd colum of the matrix
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(1 * v4s));

		//3 rd colum of the matrix
		glEnableVertexAttribArray(7);
		glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(2 * v4s));

		//4 th colum of the matrix
		glEnableVertexAttribArray(8);
		glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(3 * v4s));

		//update atribute arrays, 3, 4, 5, 6 each instace 
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glVertexAttribDivisor(7, 1);
		glVertexAttribDivisor(8, 1);
		glBindVertexArray(0);
	}

	//------------------
	// DEPTH MAP TEXTURE
	//------------------
	//resolution of the depth map
	const unsigned int SHADOW_HEIGHT = 2024, SHADOW_WIDTH = 2024;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//NOTE how we set a texture type to be GL_DEPTH_COMPONENT instead GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	//set the textures prameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	//-------------------
	// SHADOW MAPPING FBO
	//-------------------
	unsigned int depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);

	//attatch texture to the frame buffer depth value
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	//we are not going to need the color buffer
	//we tell this to openGl like so
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindBuffer(GL_FRAMEBUFFER, 0);


	//-----------------
	//MAIN FRAME BUFFER
	//-----------------
	unsigned int fbo;
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer(GL_FRAMEBUFFER, fbo);

	unsigned int fboTexture;
	glGenTextures(1, &fboTexture);
	glBindTexture(GL_TEXTURE_2D, fboTexture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//render buffer object for depth and stencil values
	unsigned int rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);


	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "-------------FRAME BUFFER BOUND SUCCESSFULLY-----------------\n";
	}
	else
		std::cout << "ERROR:BUFFER:FRAME\n Frame buffer not bound successfully \n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//---------------
	// G-FRAME BUFFER
	//---------------
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindBuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gColorSpec;

	//positions
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

	//normals
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	
	//colors and shinines
	glGenTextures(1, &gColorSpec);
	glBindTexture(GL_TEXTURE_2D, gColorSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gNormal, 0);

	unsigned int colorBuffersAttachments[3] = { GL_COLOR_ATTACHMENT0,GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3,colorBuffersAttachments);

	unsigned int gRboDepth;
	glGenRenderbuffers(1, &gRboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, gRboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, gRboDepth);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
		std::cout << "-------------FRAME BUFFER BOUND SUCCESSFULLY-----------------\n";
	else
		std::cout << "ERROR:BUFFER:FRAME\n Frame buffer not bound successfully \n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//-----------------
	// TEXTURES LOADING
	//-----------------
	unsigned int floorTexture = loadTexture("Assets/Textures/AdvancedLightning/grid_w.jpg", false);
	unsigned int lightTexture = loadTexture("Assets/Textures/AdvancedLightning/light.png", false);
	unsigned int cubeTexture = loadTexture("Assets/Textures/AdvancedLightning/cube-wood.jpg", false);
	unsigned int brickWall = loadTexture("Assets/Textures/AdvancedLightning/brickwall.jpg", false);
	unsigned int normalMap = loadTexture("Assets/Textures/AdvancedLightning/brickwall_normal.jpg", false);
	unsigned int floorNormalMap = loadTexture("Assets/Textures/AdvancedLightning/floor_normal.jpg", false);

	floorShader.use();
	floorShader.setInt("texture_diffuse0", 0);
	floorShader.setInt("shadowMap", 1);

	lightSourceShader.use();
	lightSourceShader.setInt("lightTexture", 0);

	finalShaderStage.use();
	finalShaderStage.setInt("gPosition", 0);
	finalShaderStage.setInt("gNormal", 1);
	finalShaderStage.setInt("gColorAndShinines", 2);
	//===================================== RENDER LOOP ================================================//

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// input
		// -----
		processInput(window);

		//--------------------------------------//
		//------------- DEPTH MAP -------------//
		//------------------------------------//

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		glCullFace(GL_FRONT);
		// configure projection matrix
		float nearPlane, farPlane;
		nearPlane = 1.0f;
		farPlane = 10.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

		//configure view matrix
		glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		//combine them together to get the matrix that transfoms coordinates from view space to light space
		// in the notes as T 
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		//draw the scene to the depth map
		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		//stormtrooper.Draw(shadowMapShader);
		glCullFace(GL_BACK);

		//--------------------------------------//
		//---------- NORMAL SCENE -------------//
		//------------------------------------//
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.101f, 0.101f, 0.101f, 1.0f);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		//---------------------
		// SET LIGHT PROPERTIES
		//----------------------
		gBufferShader.use();
		hasNormalMap = true;
		gBufferShader.setFloat("hasNormalMap", hasNormalMap);
		
		//---------------
		// DRAW THE MODEL
		//---------------
		setMatrices(gBufferShader, model, view, projection);
		stormtrooper.DrawInstaced(gBufferShader);

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		//----------------------
		// DRAW PLANE AS A FLOOR
		//----------------------
		/*floorShader.use();
		useTexture(0, floorTexture);
		useTexture(1, depthMap);
		floorShader.setMat4("lightMatrix", lightSpaceMatrix);
		floorShader.setVec3("lightPos", lightPosition);
		floorShader.setVec3("lightColor", lightColor);
		floorShader.setVec3("viewPos", camera.Position);
		DrawPlane(floorShader, model, view, projection, planeVAO);*/

		//---------------------------
		//RENDER THE QUAD AS A SCREEN
		//---------------------------

		//send light position uniform
		//and setup light uniform

		for (int i = 0; i < NR_LIGHTS; i++)
		{
			finalShaderStage.setVec3("lights[" + std::to_string(i) + "].position", lightPositions[i]);
			finalShaderStage.setVec3("lights[" + std::to_string(i) + "].color", lightColors[i]);
		}
		finalShaderStage.setVec3("viewPos", camera.Position);

		//----------------------
		// DRAW THE LIGHT SOURCE
		//----------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.6f));
		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", lightColor);
		useTexture(0, lightTexture);
		setMatrices(lightSourceShader, model, view, projection);
		DrawPlane(lightSourceShader, model, view, projection, lightVAO);
	
		useTexture(0, gPosition);
		useTexture(1, gNormal);
		useTexture(2, gColorSpec);
		DrawPlane(finalShaderStage, glm::mat4(0.0f), glm::mat4(0.0f), glm::mat4(0.0f), screenQuadVAO, GL_TRIANGLE_STRIP, 4);

		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	glfwTerminate();

	return 0;
}

//function that will handle resizing of the window
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);


	const float lightSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		lightPosition.z += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		lightPosition.z -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		lightPosition.x += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		lightPosition.x -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
		lightPosition.y -= lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		lightPosition.y += lightSpeed;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS)
		hasNormalMap = 1.0f;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
		hasNormalMap = 0.0f;

}

float opacityOfTexture(GLFWwindow* window, Shader shader) {
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
		return 0.001f;
	}
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
		return -0.001f;
	}
	return 0;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	
	if (firstMouse) // initially set to true
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = ypos - lastY; //calculate how much does mouse move 

	lastX = xpos;
	lastY = ypos; //update last mouse position 

	camera.ProcessMouseMovement(xOffset, yOffset);

}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
