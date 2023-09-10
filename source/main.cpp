
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
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;
// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = 800.0f / 2.0f;
float lastY = 600.0f / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

glm::vec3 lightColor = colorOf(241.0f, 180.0f, 87.0f);

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;

bool isLightBlinn = true;

//X:  0.727711f
//Y : 0.181431f
//Z : 3.52413f
glm::vec3 lightPosition(-2.0f, 2.0f, -1.0f);



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

	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
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


	Shader mainObjectShader("VertexShader/AdvancedLightning/HDRvertex.glsl", "FragmentShader/AdvancedLightning/HDRfragment.glsl");

	Shader lightSourceShader("VertexShader/AdvancedLightning/LightSourceVertex.glsl", "FragmentShader/AdvancedLightning/LightSourceFragment.glsl");

	Shader brickWallShader("VertexShader/AdvancedLightning/WoodenCubeVertex.glsl", "FragmentShader/AdvancedLightning/WoodenCubeFragment.glsl");

	Shader shadowMapShader("VertexShader/AdvancedLightning/ShadowMapVertex.glsl", "FragmentShader/AdvancedLightning/ShadowMapFragement.glsl");

	Shader floorShader("VertexShader/FloorVertex.glsl", "FragmentShader/FloorFragment.glsl");

	Shader HDRshader("VertexShader/AdvancedLightning/FBOvertex.glsl", "FragmentShader/AdvancedLightning/FBOfragment.glsl");

	stbi_set_flip_vertically_on_load(true);

	// plane VAO
	unsigned int planeVAO = createVAO(planeVertices, sizeof(planeVertices) / sizeof(float));


	//VBO, EBO and VAO for the square that represents light position
	unsigned int lightVAO = createVAO(lightVertices, sizeof(lightVertices) / sizeof(float), false);

	//cube VAO
	unsigned int cubeVAO = createVAO(cubeVertices, sizeof(cubeVertices) / sizeof(float));

	//Plane where to render framebufer
	unsigned int hdrPlaneVAO = createVAO(HDRframeBufferVertecies, sizeof(HDRframeBufferVertecies) / sizeof(float), false);

	std::vector <glm::vec3> lightColors;
	lightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	lightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	lightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	lightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

	std::vector<glm::vec3> pointLightPositions;
	pointLightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	pointLightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	pointLightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	pointLightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));

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
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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

	//------------------
	// HDR FRAME BUFFER
	//------------------
	
	//floating point color buffer
	unsigned int HDRtexture;
	glGenTextures(1, &HDRtexture);
	glBindTexture(GL_TEXTURE_2D, HDRtexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//depth render buffer
	unsigned int HDRrbo;
	glGenRenderbuffers(1, &HDRrbo);
	glBindRenderbuffer(GL_RENDERBUFFER, HDRrbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	
	unsigned int HDRfbo;
	glGenFramebuffers(1, &HDRfbo);
	glBindFramebuffer(GL_FRAMEBUFFER, HDRfbo);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, HDRtexture, 0);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, HDRrbo);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "ERROR::FRAME BUFFER::COULD NOT CREATE FRAME BUFFER";
	}
	else
		std::cout << "FRAMEBUFFER created successfully";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//-----------------
	// TEXTURES LOADING
	//-----------------
	unsigned int floorTexture = loadTexture("Assets/Textures/AdvancedLightning/grid_w.jpg", false);
	unsigned int lightTexture = loadTexture("Assets/Textures/AdvancedLightning/light.png", false);
	unsigned int cubeTexture = loadTexture("Assets/Textures/AdvancedLightning/wood.png", true);
	unsigned int brickWall = loadTexture("Assets/Textures/AdvancedLightning/toy_box_diffuse.png", false);
	unsigned int normalMap = loadTexture("Assets/Textures/AdvancedLightning/toy_box_normal.png", false);
	unsigned int heightMap = loadTexture("Assets/Textures/AdvancedLightning/toy_box_disp.png", false);
	unsigned int floorNormalMap = loadTexture("Assets/Textures/AdvancedLightning/floor_normal.jpg", false);

	mainObjectShader.use();
	mainObjectShader.setInt("texture_diffuse0", 0);
	mainObjectShader.setInt("shadowMap", 1);
	mainObjectShader.setInt("texture_normal0", 2);
	mainObjectShader.setInt("heightMap", 3);

	lightSourceShader.use();
	lightSourceShader.setInt("lightTexture", 0);

	brickWallShader.use();
	brickWallShader.setInt("texture_diffuse0", 0);
	brickWallShader.setInt("shadowMap", 1);

	floorShader.use();
	floorShader.setInt("texture_diffuse0", 0);
	floorShader.setInt("shadowMap", 1);

	HDRshader.use();
	HDRshader.setInt("hdrBuffer", 0);
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

		glm::mat4 lightModel = glm::mat4(1.0f);
		DrawShadowMapCube(shadowMapShader, lightModel, cubeVAO);
		shadowMapShader.setMat4("model", lightModel);
		
		glCullFace(GL_BACK);
		//--------------------------------------//
		//---------- NORMAL SCENE -------------//
		//------------------------------------//

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, HDRfbo);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glClearColor(0.101f, 0.101f, 0.101f, 1.0f);

			glm::mat4 view = camera.GetViewMatrix();
			glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
			glm::mat4 model = glm::mat4(1.0f);

			//----------------------
			// DRAW PLANE AS A FLOOR
			//----------------------
			useTexture(0, floorTexture);
			floorShader.use();
			floorShader.setVec3("directionLight.position", lightPosition);
			floorShader.setVec3("directionLight.color", COLOR_WHITE);
			floorShader.setVec3("viewPos", camera.Position);
			floorShader.setMat4("lightMatrix", lightSpaceMatrix);

			for (int i = 0; i < 4; i++)
			{
				floorShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
				floorShader.setVec3("pointLights[" + std::to_string(i) + "].color", lightColors[i]);	
				floorShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
				floorShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
				floorShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
			}

			

			useTexture(1, depthMap);
			DrawPlane(floorShader, model, view, projection, planeVAO);

			//---------------
			// DRAW THE PLANE
			//---------------
			mainObjectShader.use();
			mainObjectShader.setVec3("lightPos", lightPosition);
			mainObjectShader.setVec3("viewPos", camera.Position);

			for (int i = 0; i < 4; i++)
			{
				mainObjectShader.setVec3("pointLights[" + std::to_string(i) + "].position", pointLightPositions[i]);
				mainObjectShader.setVec3("pointLights[" + std::to_string(i) + "].color", lightColors[i]);
				
				mainObjectShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.09f);
				mainObjectShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
				mainObjectShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.032f);
			}
		
			useTexture(0, cubeTexture);
			DrawCube(mainObjectShader, model, view, projection, cubeVAO);
			//----------------------
			// DRAW THE LIGHT SOURCE
			//----------------------
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPosition);
			model = glm::scale(model, glm::vec3(0.5f));
			lightSourceShader.use();
			for (int i = 0; i < 4; i++)
			{
				lightSourceShader.setVec3("lightColor", lightColors[i]);

				model = glm::mat4(1.0f);
				model = glm::translate(model, pointLightPositions[i]);

				useTexture(0, lightTexture);
				DrawPlane(lightSourceShader, model, view, projection, lightVAO);
	
			}
			model = glm::mat4(1.0f);
			model = glm::translate(model, lightPosition);
			lightSourceShader.setVec3("lightColor", lightColor);
			DrawPlane(lightSourceShader, model, view, projection, lightVAO);


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		HDRshader.use();
		HDRshader.setFloat("exposure", 0.3f);
		useTexture(0, HDRtexture);
		glBindVertexArray(hdrPlaneVAO);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindVertexArray(0);
		
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();

	}

	std::cout << "Lights coordniates are \n X:" << lightPosition.x << "f\n Y:" << lightPosition.y << "f\n Z:" << lightPosition.z << "f\n";

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
