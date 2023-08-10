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

glm::vec3 diffusePoint = COLOR_SUN;

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;

bool isLightBlinn = true;


//light possition
glm::vec3 lightPosition(2.0, 0.4, 0.0);

glm::vec3 pointLightPositions[] = {
glm::vec3(0.7f, 0.2f, 2.0f),
glm::vec3(2.3f, -3.3f, -4.0f),
glm::vec3(-4.0f, 2.0f, -12.0f),
glm::vec3(0.0f, 0.0f, -3.0f)
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

	// plane VAO
	unsigned int planeVAO, planeVBO;
	glGenVertexArrays(1, &planeVAO);
	glGenBuffers(1, &planeVBO);
	glBindVertexArray(planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glBindVertexArray(0);


	//VBO, EBO and VAO for the square that represents light position
	unsigned int lightVAO, lightVBO;
	glGenVertexArrays(1, &lightVAO);
	glGenBuffers(1, &lightVBO);
	glBindVertexArray(lightVAO);
	glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(lightVertices), &lightVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glBindVertexArray(0);

	unsigned int floorTexture = loadTexture("Assets/Textures/AdvancedLightning/wood.png", true);
	unsigned int lightTexture = loadTexture("Assets/Textures/AdvancedLightning/light.png", true);


	shader.use();
	shader.setInt("floorTexture", 0);

	lightSourceShader.use();
	shader.setInt("lightTexture", 0);

	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.101f, 0.101f, 0.101f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// input
		// -----
		processInput(window);

		shader.use();
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		shader.setVec3("lightPos",lightPosition);
		shader.setVec3("lightColor", colorOf(241.0f, 180.0f, 87.0f));
		shader.setVec3("viewPos", camera.Position);
		shader.setVec3("specularColor", colorOf(241.0f, 130.0f, 80.0f));

		shader.setBool("blinnModel", isLightBlinn);
		
		//----------------------
		// DRAW PLANE AS A FLOOR
		//----------------------
		glBindVertexArray(planeVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, floorTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	
		glBindVertexArray(0);

		//----------------------
		// DRAW THE LIGHT SOURCE
		//----------------------
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, lightPosition);
		model = glm::scale(model, glm::vec3(0.6f));
		lightSourceShader.use();
		lightSourceShader.setMat4("view", view);
		lightSourceShader.setMat4("projection", projection);
		lightSourceShader.setMat4("model", model);
		lightSourceShader.setVec3("lightColor", colorOf(241.0f, 180.0f, 87.0f));

		glBindVertexArray(lightVAO);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, lightTexture);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

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
