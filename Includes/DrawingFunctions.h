#pragma once
#include <iostream>
#include "Shader.h"
#include "HelperFunctions.h"
#include <glm/glm.hpp>

/// <summary>
/// Draws the cube on the screen
/// </summary>
/// <param name="shader">Shader program that will be use when drawing the cube</param>
/// <param name="model">Model matrix that will be used NOTE: name of uniform must be "model"</param>
/// <param name="view">View matrix that will be used NOTE: name of uniform must be "view"</param>
/// <param name="projection">Projection matrix that will be used NOTE: name of uniform must be "projection"</param>
/// <param name="VAO">VAO that holds data for rendering</param>
/// <param name="texture">actual texture image to use</param>
/// <param name="textureSamplerNumber">texture image sampler number</param>
/// <param name=""></param>
void DrawCube(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int VAO, unsigned int texture, unsigned int textureSamplerNumber)
{
	shader.use();
	glBindVertexArray(VAO);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE+textureSamplerNumber);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 36);

	glBindVertexArray(0);
}

/// <summary>
/// Draws plane on the scene
/// </summary>
/// <param name="shader">Shader program that will be use when drawing the cube</param>
/// <param name="model">Model matrix that will be used NOTE: name of uniform must be "model"</param>
/// <param name="view">View matrix that will be used NOTE: name of uniform must be "view"</param>
/// <param name="projection">Projection matrix that will be used NOTE: name of uniform must be "projection"</param>
/// <param name="VAO">VAO that holds data for rendering</param>
/// <param name="texture">actual texture image to use</param>
/// <param name="textureSamplerNumber">texture image sampler number</param>
/// <param name=""></param>
void DrawPlane(Shader shader, glm::mat4 model, glm::mat4 view, glm::mat4 projection, unsigned int VAO, unsigned int texture, unsigned int textureSamplerNumber)
{
	shader.use();
	glBindVertexArray(VAO);

	shader.setMat4("model", model);
	shader.setMat4("view", view);
	shader.setMat4("projection", projection);

	glActiveTexture(GL_TEXTURE + textureSamplerNumber);
	glBindTexture(GL_TEXTURE_2D, texture);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glBindVertexArray(0);
}