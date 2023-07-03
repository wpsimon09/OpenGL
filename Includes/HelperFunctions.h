#pragma once
#include <iostream>
#include<glm/glm.hpp>


glm::vec3 colorOf(float r, float g, float b) {
	glm::vec3 color;
	color.x = r / 255.0f;
	color.y = g / 255.0f;
	color.z = b / 255.0f;

	return color;
}					  
#define COLOR_RED glm::vec3(1.0f, 0.0f, 0.0f)
#define COLOR_GREEN glm::vec3(0.0f, 1.0f, 0.0f)
#define COLOR_BLUE glm::vec3(0.0f, 0.0f, 1.0f)
#define COLOR_YELLOW glm::vec3(1.0f, 1.0f, 0.0f)
#define COLOR_MAGENTA glm::vec3(1.0f, 0.0f, 1.0f)
#define COLOR_CYAN glm::vec3(0.0f, 1.0f, 1.0f)
#define COLOR_WHITE glm::vec3(1.0f, 1.0f, 1.0f)
#define COLOR_BLACK glm::vec3(0.0f, 0.0f, 0.0f)
#define COLOR_SUN colorOf(253.0f, 251.0f, 211.0f);
