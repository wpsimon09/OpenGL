#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include "Shader.h";

struct DirLight {
	glm::vec3 direction;

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

struct PointLight {
	float constant;
	float linear;
	float quadratic;

	glm::vec3 position;

	glm::vec3 color;
};

struct SpotLight {
	float cutOff;
	float outerCutOff;

	float constant;
	float linear;
	float quadratic;

	bool isOn;

	glm::vec3 position;
	glm::vec3 direciton;
	
	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;
};

class Light  
{
	public:
		Light(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);
		Light(float constant, float linear, float quadratic, glm::vec3 position, glm::vec3 color, std::string structName, Shader shader);
		Light(float cutOff, float outerCutOFF,float constant, float linear, float quadratic, bool isOn, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

		void setLight(Shader &shader, std::string structName);

		std::string getType();
	private:
		PointLight _pointLight;
		DirLight _dirLight;
		SpotLight _spotLight;
		std::string lightType;
};


Light::Light(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) {
	this->_dirLight.direction = direction;
	this->_dirLight.ambient = ambient;
	this->_dirLight.diffuse = diffuse;
	this->_dirLight.specular = specular;

	this->lightType = "dir";
}

Light::Light(float cutOff, float outerCutOFF, float constant, float linear, float quadratic,bool isOn, glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->_spotLight.constant = constant;
	this->_spotLight.linear = linear;
	this->_spotLight.quadratic = quadratic;

	this->_spotLight.isOn = isOn;
	this->_spotLight.position = position;
	
	this->_spotLight.ambient = ambient;
	this->_spotLight.diffuse = diffuse;
	this->_spotLight.specular = specular;

	this->lightType = "spot";
}

Light::Light(float constant, float linear, float quadratic, glm::vec3 position, glm::vec3 color, std::string structName, Shader shader) {
	this->_pointLight.constant = constant;
	this->_pointLight.linear = linear;
	this->_pointLight.quadratic = quadratic;
	this->_pointLight.position = position;
	this->_pointLight.color = color;


	this->lightType = "point";

	this->setLight(shader, structName);
}

void Light::setLight(Shader& shader, std::string structName) {
	shader.use();
	shader.setFloat("material.shininess", 32);
	if (this->lightType == "dir")
	{
		shader.setVec3(structName+".direction", this->_dirLight.direction);
		shader.setVec3(structName+".ambient", this->_dirLight.ambient);
		shader.setVec3(structName+".diffuse", this->_dirLight.diffuse);
		shader.setVec3(structName+".specular", this->_dirLight.specular);
	}

	if(this->lightType == "point")
	{
		shader.setFloat(structName + ".constant", this->_pointLight.constant);
		shader.setFloat(structName + ".linear", this->_pointLight.linear);
		shader.setFloat(structName + ".quadratic", this->_pointLight.quadratic);

		shader.setVec3(structName + ".position", this->_pointLight.position);
		
		shader.setVec3(structName + ".color", this->_pointLight.color);

	}

	if (this->lightType == "spot")
	{
		shader.setFloat(structName+ ".constant", this->_spotLight.constant);
		shader.setFloat(structName+ ".linear", this->_spotLight.linear);
		shader.setFloat(structName+ ".quadratic", this->_spotLight.quadratic);
		
		shader.setBool(structName + ".isOn", this->_spotLight.isOn);
		shader.setVec3(structName + ".position", this->_spotLight.position);
		
		shader.setVec3(structName + ".ambient", this->_spotLight.ambient);
		shader.setVec3(structName + ".diffuse", this->_spotLight.diffuse);
		shader.setVec3(structName + ".specular", this->_spotLight.specular);
	}
}


std::string Light::getType() {
	std::cout << this->lightType;
	return this->lightType;
}


