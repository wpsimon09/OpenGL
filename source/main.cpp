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


//light possition
glm::vec3 lightPosition(-2.0f, 4.0f, -1.0f);

glm::vec3 pointLightPositions[] = {
glm::vec3(0.7f, 0.2f, 2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f, 2.0f, -12.0f),
glm::vec3(0.0f, 0.0f, -3.0f)
};

glm::vec3 cubePostions[] = {
	glm::vec3(0.0f, 1.5f, 0.0),
	glm::vec3(2.0f, 0.0f, 1.0),
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

	stbi_set_flip_vertically_on_load(true);

	Shader shader("VertexShader/AdvancedLightning/AdvancedLightningVertex.glsl", "FragmentShader/AdvancedLightning/AdvancedLightningFragmnet.glsl");

	Shader lightSourceShader("VertexShader/AdvancedLightning/LightSourceVertex.glsl", "FragmentShader/AdvancedLightning/LightSourceFragment.glsl");

	Shader woodenCubeShader("VertexShader/AdvancedLightning/WoodenCubeVertex.glsl", "FragmentShader/AdvancedLightning/WoodenCubeFragment.glsl");

	Shader shadowMapShader("VertexShader/AdvancedLightning/ShadowMapVertex.glsl", "FragmentShader/AdvancedLightning/ShadowMapFragement.glsl");

	// plane VAO
	unsigned int planeVAO = createVAO(planeVertices, sizeof(planeVertices)/sizeof(float));


	//VBO, EBO and VAO for the square that represents light position
	unsigned int lightVAO = createVAO(lightVertices,sizeof(lightVertices)/sizeof(float), false);

	//cube VAO
	unsigned int cubeVAO = createVAO(cubeVertices, sizeof(cubeVertices) / sizeof(float));;


	//------------------
	// DEPTH MAP TEXTURE
	//------------------
	//resolution of the depth map
	const unsigned int SHADOW_HEIGHT = 1024, SHADOW_WIDTH = 1024;
	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	//NOTE how we set a texture type to be GL_DEPTH_COMPONENT instead GL_RGB or GL_RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	//set the textures prameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

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
	// TEXTURES LOADING
	//-----------------
	unsigned int floorTexture = loadTexture("Assets/Textures/AdvancedLightning/wood.png", true);
	unsigned int lightTexture = loadTexture("Assets/Textures/AdvancedLightning/light.png", true);
	unsigned int cubeTexture = loadTexture("Assets/Textures/AdvancedLightning/cube-wood.jpg", false);

	shader.use();
	shader.setInt("wood", 0);
	shader.setInt("shadowMap", 1);

	lightSourceShader.use();
	lightSourceShader.setInt("lightTexture", 0);

	woodenCubeShader.use();
	woodenCubeShader.setInt("woodCube", 0);

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
		glClear(GL_DEPTH_BUFFER_BIT| GL_COLOR_BUFFER_BIT);


		// configure projection matrix
		float nearPlane, farPlane;
		nearPlane = 1.0f;
		farPlane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);

		//configure view matrix
		glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));

		//combine them together to get the matrix that transfoms coordinates from view space to light space
		// in the notes as T 
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;

		//draw the scene 
		shadowMapShader.use();
		shadowMapShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glm::mat4 ligthModel = glm::mat4(1.0f);
		DrawShadowMapPlane(shadowMapShader, ligthModel, planeVAO);
		for (int i = 0; i < 3; i++)
		{
			ligthModel = glm::mat4(1.0f);
			ligthModel = glm::scale(ligthModel, glm::vec3(0.4f));
			ligthModel = glm::translate(ligthModel, cubePostions[i]);
			if (i == 2)
			{
				ligthModel = glm::rotate(ligthModel, (float)glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				ligthModel = glm::scale(ligthModel, glm::vec3(0.25));
			}
			DrawShadowMapCube(shadowMapShader, ligthModel, cubeVAO);
		}

		//--------------------------------------//
		//---------- NORMAL SCENE -------------//
		//------------------------------------//

		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(0.101f, 0.101f, 0.101f, 1.0f);

		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 model = glm::mat4(1.0f);

		//---------------------
		// SET LIGHT PROPERTIES
		//---------------------
		shader.use();
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);

		shader.setVec3("lightPos",lightPosition);
		shader.setVec3("lightColor", lightColor);
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("specularColor", lightColor);
		shader.setMat4("lightMatrix", lightSpaceMatrix);

		//----------------------
		// DRAW PLANE AS A FLOOR
		//----------------------
		DrawPlane(shader, model, view, projection, planeVAO);

		//-----------
		// DRAW CUBES
		//-----------
		woodenCubeShader.use();
		woodenCubeShader.setVec3("lightPos", lightPosition);
		woodenCubeShader.setVec3("lightColor", lightColor);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, cubeTexture);
		for (int i = 0; i < 3; i++)
		{
			model = glm::mat4(1.0f);
			model = glm::scale(model, glm::vec3(0.4f));
			model = glm::translate(model, cubePostions[i]);
			if (i == 2)
			{
				model = glm::rotate(model, (float)glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
				model = glm::scale(model, glm::vec3(0.25));
			}
			DrawCube(woodenCubeShader, model, view, projection, cubeVAO);
		}


		//----------------------
		// DRAW THE LIGHT SOURCE
		//----------------------
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.6f));
		lightSourceShader.use();
		lightSourceShader.setVec3("lightColor", lightColor);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightTexture);
		DrawPlane(lightSourceShader, model, view, projection, lightVAO);
	

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


	const float cameraSpeed = 2.5f * deltaTime; // adjust accordingly
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		isLightBlinn = true;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		isLightBlinn = false;

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
