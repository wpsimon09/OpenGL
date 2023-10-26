#include <glad/glad.h>
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
#include <random>

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

glm::vec3 lightColor = glm::vec3(2.5f, 2.0f,2.5f);

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;

bool isLightBlinn = true;

//light possition
glm::vec3 lightPosition(0.0f, 2.0f, -1.0f);


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
	glEnable(GL_CULL_FACE);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int rows = 10;
	unsigned int colums = 10;
	unsigned int totalAmount = rows * colums;


	Shader mainObjShader("VertexShader/AdvancedLightning/AdvancedLightningVertex.glsl", "FragmentShader/AdvancedLightning/AdvancedLightningFragmnet.glsl", "main");

	Shader lightSourceShader("VertexShader/AdvancedLightning/LightSourceVertex.glsl", "FragmentShader/AdvancedLightning/LightSourceFragment.glsl", "light sourece");

	Shader shadowMapShader("VertexShader/AdvancedLightning/ShadowMapVertex.glsl", "FragmentShader/AdvancedLightning/ShadowMapFragement.glsl", "shadow map");
	
	Shader floorShader("VertexShader/FloorVertex.glsl", "FragmentShader/FloorFragment.glsl", "floor");

	Shader gBufferShader("VertexShader/AdvancedLightning/gBufferVertex.glsl", "FragmentShader/AdvancedLightning/gBufferFragment.glsl", "gBuffer");

	Shader finalShaderStage("VertexShader/AdvancedLightning/FinalVertex.glsl", "FragmentShader/AdvancedLightning/FinalFragment.glsl", "final shader");

	Shader ssaoShader("VertexShader/AdvancedLightning/SSAOVertex.glsl", "FragmentShader/AdvancedLightning/SSAOFragment.glsl", "SSAOShader");

	Shader ssaoBlurShade("VertexShader/AdvancedLightning/SSAOVertex.glsl", "FragmentShader/AdvancedLightning/SSAOBlurFragment.glsl", "SSAO shade blur");

	Model stormtrooper("Assets/Model/stormtrooper/stormtrooper.obj", totalAmount);

	stbi_set_flip_vertically_on_load(true);

	//-------------------------
	// HEMISPHERE SAMPLE KERNEL
	//-------------------------
	std::uniform_real_distribution<float> randomFloats(0.15, 0.8);
	std::default_random_engine generator;
	std::vector<glm::vec3> ssaoKernel;
	for (unsigned int i = 0; i < 64; ++i)
	{
		glm::vec3 sample(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator)
		);
		sample = glm::normalize(sample);
		sample *= randomFloats(generator);
		
		//distribute more  samples around fragment we are sampling around (origin)
		float scale = (float)i / 64.0f;
		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.push_back(sample);
	}

	//-----------------
	// ROTATION VECTORS
	//-----------------
	std::vector<glm::vec3> ssaoRotations;
	for (unsigned int i = 0; i < 16; ++i)
	{
		glm::vec3 noise(
			randomFloats(generator) * 2.0 - 1.0,
			randomFloats(generator) * 2.0 - 1.0,
			0.0f
		);
		ssaoRotations.push_back(noise);
	}

	// plane VAO
	unsigned int floorVAO = createVAO(planeVertices, sizeof(planeVertices)/sizeof(float));

	//final frame buffer VAO
	unsigned int screenQuadVAO = createVAO(HDRframeBufferVertecies, sizeof(planeVertices) / sizeof(float), false, true);

	//VBO, EBO and VAO for the square that represents light position
	unsigned int lightVAO = createVAO(lightVertices,sizeof(lightVertices)/sizeof(float), false);

	//cube VAO
	unsigned int cubeVAO = createVAO(cubeVertices, sizeof(cubeVertices) / sizeof(float));

	//wall VAO
	unsigned int wallVAO = createVAO(wallVertecies, sizeof(wallVertecies) / sizeof(float));

	//-----------------------
	// SSAO ROTATIONS TEXTURE
	//-----------------------
	unsigned int noiseTexture;
	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoRotations[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	//--------------
	// SSAO BLUR FBO
	//--------------
	unsigned int ssaoBlurFBO, ssaoBlurColorBuffer;
	glGenFramebuffers(1, &ssaoBlurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
	glGenTextures(1, &ssaoBlurColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoBlurColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,ssaoBlurColorBuffer, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	
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

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "-------------FRAME BUFFER BOUND SUCCESSFULLY-----------------\n";
	}
	else
		std::cout << "ERROR:BUFFER:FRAME\n Frame buffer not bound successfully \n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	//===============
	// G-FRAME BUFFER
	//===============
	unsigned int gBuffer;
	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	unsigned int gPosition, gNormal, gAlbedoSpec;

	//---------
	// POSITION
	//---------
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0,	GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	
	//---------
	// NORMAL
	//---------
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	
	//-------
	// ALBEDO
	//-------
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	
	// tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// create and attach depth buffer (renderbuffer)
	unsigned int rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//------------------
	// SSAO FRAME BUFFER
	//------------------
	unsigned int ssaoFBO;
	glGenFramebuffers(1, &ssaoFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

	//texture wher ewe will stor the SSAO values
	unsigned int ssaoColorBuffer;
	glGenTextures(1, &ssaoColorBuffer);
	glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

	//-----------------
	// TEXTURES LOADING
	//-----------------
	unsigned int floorTexture = loadTexture("Assets/Textures/AdvancedLightning/grid_w.jpg", false);
	unsigned int lightTexture = loadTexture("Assets/Textures/AdvancedLightning/sun.png", false);
	unsigned int cubeTexture = loadTexture("Assets/Textures/AdvancedLightning/cube-wood.jpg", false);
	unsigned int brickWall = loadTexture("Assets/Textures/AdvancedLightning/brickwall.jpg", false);
	unsigned int normalMap = loadTexture("Assets/Textures/AdvancedLightning/brickwall_normal.jpg", false);
	unsigned int floorNormalMap = loadTexture("Assets/Textures/AdvancedLightning/floor_normal.jpg", false);
	unsigned int defaultTexture = loadTexture("Assets/Textures/default.jpg", false);

	floorShader.use();
	floorShader.setInt("texture_diffuse0", 0);
	floorShader.setInt("shadowMap", 1);

	lightSourceShader.use();
	lightSourceShader.setInt("lightTexture", 0);

	finalShaderStage.use();
	finalShaderStage.setInt("gPosition", 0);
	finalShaderStage.setInt("gNormal", 1);
	finalShaderStage.setInt("gAlbedoSpec", 2);
	finalShaderStage.setInt("shadowMap", 3);
	finalShaderStage.setInt("ssaoEffect", 4);

	ssaoShader.use();
	ssaoShader.setInt("gPosition", 0);
	ssaoShader.setInt("gNormal", 1);
	ssaoShader.setInt("texNoise", 2);
	

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
		//------------- SHADOW PASS ------------//
		//--------------------------------------//

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		
		// configure projection matrix
		float nearPlane, farPlane;
		nearPlane = 1.0f;
		farPlane = 15.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

		//configure view matrix
		glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		//combine them together to get the matrix that transfoms coordinates from view space to light space
		// in the notes as T 
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		glm::mat4 lightModel = glm::mat4(1.0f);

		//draw the scene to the depth map
		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		lightModel = glm::translate(lightModel, glm::vec3(0.0f, 0.4f, 0.0f));

		//-----------------
		// DRAW SHAOW MODEL
		//-----------------
		glCullFace(GL_FRONT);
		setMatrices(shadowMapShader, lightModel, lightView, lightProjection);
		stormtrooper.Draw(shadowMapShader);
		
		glDisable(GL_CULL_FACE);
		lightModel = glm::mat4(1.0f);
		lightModel = glm::translate(lightModel, glm::vec3(-2.0f, 0.0f, -2.0f));
		lightModel = glm::scale(lightModel, glm::vec3(5.0f));
		DrawShadowMapPlane(shadowMapShader, lightModel, wallVAO);

		//--------------------------------------//
		//---------- GEOMETRY PASS ------------//
		//------------------------------------//

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
		glClearColor(0.1, 0.1f, 0.1f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_CULL_FACE);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		//-----------------
		// SET NRORMAL MAPS
		//-----------------
		gBufferShader.use();
		gBufferShader.setFloat("hasNormalMap", hasNormalMap);
		
		//---------------
		// DRAW THE MODEL
		//---------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0.0f, 0.4f, 0.0f));
		glCullFace(GL_BACK);
		setMatrices(gBufferShader, model, view, projection);
		stormtrooper.Draw(gBufferShader);

		//--------------------------------------//
		//-------------- SSAO PASS ------------//
		//------------------------------------//
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
		glClear(GL_COLOR_BUFFER_BIT );
		glClearColor(0.1, 0.1f, 0.1f, 0.0f);

		ssaoShader.use();

		//-------------
		// SET UNIFORMS
		//-------------
		for (int i = 0; i < 64; i++)
		{
			ssaoShader.setVec3("samples[" + std::to_string(i) + "]", ssaoKernel[i]);
		}
		useTexture(0, gPosition);
		useTexture(1, gNormal);
		useTexture(2, noiseTexture);

		ssaoShader.setBool("hasNormalMap", false);
		
		DrawPlane(ssaoShader, glm::mat4(0.0f), glm::mat4(0.0f), projection, screenQuadVAO, GL_TRIANGLE_STRIP, 4);


		//--------------------------------------//
		//---------- SSAO BLUR PASS -----------//
		//------------------------------------//
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
		glClear(GL_COLOR_BUFFER_BIT);
		
		ssaoBlurShade.use();
		useTexture(0, ssaoColorBuffer);
		DrawPlane(ssaoBlurShade, glm::mat4(1.0f), glm::mat4(1.0f), glm::mat4(1.0f), screenQuadVAO, GL_TRIANGLE_STRIP, 4);
        
		//--------------------------------------//
		//----------   LIGHT PASS    ----------//
		//------------------------------------//

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.1, 0.1f, 0.1f, 0.0f);

		finalShaderStage.use();
		Light mainpointLight(constant, linear, quadratic, lightPosition, lightColor, "light", finalShaderStage);
		finalShaderStage.setVec3("viewPos", camera.Position);
		finalShaderStage.setMat4("lightMatrix", lightSpaceMatrix);

		useTexture(0, gPosition);
		useTexture(1, gNormal);
		useTexture(2, gAlbedoSpec);
		useTexture(3, depthMap);
		useTexture(4, ssaoBlurColorBuffer);
		
		//-----------------------------------
		// DRAW SCEEN QUAD FILLING THE SCREEN
		//-----------------------------------
		DrawPlane(finalShaderStage, glm::mat4(0.0f), glm::mat4(0.0f), glm::mat4(0.0f), screenQuadVAO, GL_TRIANGLE_STRIP, 4);
		
		glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
		
		glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		//----------------------
		// DRAW THE LIGHT SOURCE
		//----------------------
		glDisable(GL_CULL_FACE);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.6f));
		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", lightColor);
		useTexture(0, lightTexture);
		setMatrices(lightSourceShader, model, view, projection);
		DrawPlane(lightSourceShader, model, view, projection, lightVAO);
		finalShaderStage.use();

		glEnable(GL_CULL_FACE);
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
