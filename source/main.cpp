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

// light properties
glm::vec3 direction = glm::vec3(0.2f, -1.0f, -0.6f);
glm::vec3 ambient = glm::vec3(0.4f);
glm::vec3 diffuse = COLOR_SUN;
glm::vec3 specular = glm::vec3(1.0F);

glm::vec3 diffusePoint = COLOR_SUN;

float constant = 1.0f;
float linear = 0.22f;
float quadratic = 0.20f;


//light possition
glm::vec3 lightPos(0.0f, 0.0f, 2.0f);

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
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetScrollCallback(window, scroll_callback);


	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}


	Shader shader("VertexShader/PlanetVertex.glsl", "FragmentShader/PlanetFragment.glsl");

	Shader skyboxShader("VertexShader/SkyBoxVertex.glsl", "FragmentShader/SkyBoxFragment.glsl");

	Shader asteroidShader("VertexShader/AsteroidVertex.glsl", "FragmentShader/AsteroidFragment.glsl");


	//----------------------------------------------------
	// PLACE ASTEROIDS IN RANDOM ORDER AROUND THE PLANNET
	// WITH THIS CALCULATIONS WE CAN MODIFY MODEL MATRIX
	// TO MADE THE ILUSION OF A CIRCLE
	//----------------------------------------------------
	unsigned int amount = 5000;
	glm::mat4* modelMatrices;
	modelMatrices = new glm::mat4[amount];
	srand(glfwGetTime()); // initialize random seed
	float radius = 27.0;
	float offset = 12.5f;
	for (unsigned int i = 0; i < amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. translation: displace along circle with radius [-offset, offset]
		float angle = (float)i / (float)amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f -offset;

		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

		float y = displacement * 0.4f; // keep height of field smaller than x/z
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;

		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. scale: scale between 0.05 and 0.25f
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. rotation: add random rotation around a (semi)random rotation axis
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));
		// 4. now add to list of matrices
		modelMatrices[i] = model;
	}

	// skybox VAO
	unsigned int skyBoxVAO, skyBoxVBO;
	glGenVertexArrays(1, &skyBoxVAO);
	glGenBuffers(1, &skyBoxVBO);
	glBindVertexArray(skyBoxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyBoxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyBoxVertecies), &skyBoxVertecies, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);

	unsigned int skyBox = loadCubeMaps(skyboxTextures);
	stbi_set_flip_vertically_on_load(true);

	Model asteroid("Assets/Model/asteroid/rock.obj");
	Model planet("Assets/Model/planet/planet.obj");

	unsigned int asteoroidTexture = loadTexture("Assets/Model/asteroid/rock.png");

	//create buffer
	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4),&modelMatrices[0], GL_STATIC_DRAW);

	// loop throught the meshes of the asteroid
	for (unsigned int i = 0; i < asteroid.meshes.size(); i++)
	{
		// retrieve vertex array from loaded model
		unsigned int VAO = asteroid.meshes[i].VAO;

		glBindVertexArray(VAO);

		//glm::vec4 directly translets to vec4 in gls
		std::size_t v4s = sizeof(glm::vec4);

		/// since mat 4 in GLSL is created like so
		/// ----------------------------------
		/// |   vec4 (val, val, val, val)	 |
		/// |   vec4 (val, val, val, val)	 |
		/// |   vec4 (val, val, val, val)	 |
		/// |   vec4 (val, val, val, val)	 |
		/// ----------------------------------
		/// 
		/// we have to go throught each vector and set its value equal to the coresponding 
		/// collum in the model matrix that we will use for the transformations
		/// 
		
		// 1st colum of the matrix
		glEnableVertexAttribArray(3);
		glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)0);


		//2 nd colum of the matrix
		glEnableVertexAttribArray(4);
		glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(1 * v4s));
		
		//3 rd colum of the matrix
		glEnableVertexAttribArray(5);
		glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(2 * v4s));
		
		//4 th colum of the matrix
		glEnableVertexAttribArray(6);
		glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * v4s, (void*)(3 * v4s));
		
		//update atribute arrays, 3, 4, 5, 6 each instace 
		glVertexAttribDivisor(3, 1);
		glVertexAttribDivisor(4, 1);
		glVertexAttribDivisor(5, 1);
		glVertexAttribDivisor(6, 1);
		glBindVertexArray(0);
	}

	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);


	while (!glfwWindowShouldClose(window))
	{
		// per-frame time logic
		// --------------------
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		glClearColor(0.501f, 0.501f, 0.501f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// input
		// -----
		processInput(window);

		shader.use();
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);
		shader.setMat4("model", model);

		shader.setVec3("lightDir", glm::vec3(4.32022f, 4.71866, 7.83745f));
		shader.setVec3("lightColor", glm::vec3(1.0f));

		planet.Draw(shader);

		glBindVertexArray(0);

		// this for loop is basicly an draw function in model class
		// but this time is using instaced rendering
		asteroidShader.use();
		shader.setFloat("texture_diffuse1", 0);
		asteroidShader.setMat4("view", view);
		asteroidShader.setMat4("projection", projection);

		for (int i = 0; i < asteroid.meshes.size(); i++)
		{
			glBindVertexArray(asteroid.meshes[i].VAO);
			glBindTexture(GL_TEXTURE_2D, asteoroidTexture);
			glActiveTexture(GL_TEXTURE0);
			glDrawElementsInstanced(GL_TRIANGLES, asteroid.meshes[i].indecies.size(), GL_UNSIGNED_INT, 0, amount);
		}

		//----------------
		// DRAW THE SKYBOX
		//----------------
		glDepthFunc(GL_LEQUAL);
		
		skyboxShader.use();
		view = glm::mat4(glm::mat3(camera.GetViewMatrix()));

		skyboxShader.setMat4("projection", projection);
		skyboxShader.setMat4("view", view);

		glBindVertexArray(skyBoxVAO);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skyBox);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glDepthFunc(GL_LESS);

		
		glBindVertexArray(0);
		// glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(window);
		glfwPollEvents();


		std::cout << "Last positions were";
		std::cout << "X:" <<-1 * camera.Position.x << "\n";
		std::cout << "Y:" <<-1 * camera.Position.y << "\n";
		std::cout << "Z:" <<-1 * camera.Position.z << "\n";
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
