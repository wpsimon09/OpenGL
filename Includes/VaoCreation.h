#pragma once
#include <iostream>
#include "Shader.h"
#include "HelperFunctions.h"
#include <glm/glm.hpp>

/// <summary>
/// Creates vertex array obect
/// VAOs are created under assumption that vertex atribute positions are like follows:
/// 0 - positions
/// 1 - normal vectors
/// 2 - texture coordinates
/// </summary>
/// <param name="vertecies">array of vertecies (positions, normals, texture coordinates</param>
/// <param name="numberOfComponents">number of components we are passing to the vertex shader</param>
/// <param name="hasNormals">true if triangle(s) have normal vector</param>
/// <param name="hasTexCoords">true if triangle(s) have texture coordinates</param>
/// <returns>Created vertex array object</returns>
unsigned int createVAO(float vertecies[], float numberOfComponents, bool hasNormals = true, bool hasTexCoords = true)
{
	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * numberOfComponents , vertecies, GL_STATIC_DRAW);
	
	std::cout << "Sizeof vertecies:" << sizeof(float) * numberOfComponents << "bytes" << std::endl;

	if (hasNormals && hasTexCoords)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

		std::cout << "Created object has 3 vertex atributes" << std::endl;
		std::cout << "0 - positions (vec3)" << std::endl;
		std::cout << "1 - normal vectors (vec3)" << std::endl;
		std::cout << "2 - texture coordinates (vec2)" << std::endl;
	}
	if (hasNormals && !hasTexCoords)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);					
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));

		std::cout << "Created object has 2 vertex atributes" << std::endl;
		std::cout << "0 - positions (vec3)" << std::endl;
		std::cout << "1 - normal vectors (vec3)" << std::endl;
	}
	if (!hasNormals && hasTexCoords)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		
		std::cout << "Created object has 2 vertex atributes" << std::endl;
		std::cout << "0 - positions (vec3)" << std::endl;
		std::cout << "2 - texture coordinates (vec2)" << std::endl;
	}
	if (!hasNormals && !hasTexCoords)
	{
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		
		std::cout << "Created object has 1 vertex atribute" << std::endl;
		std::cout << "0 - positions (vec3)" << std::endl;
	}

	std::cout << std::endl;
	glBindVertexArray(0);

	return VAO;
}