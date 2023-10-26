#pragma once
#include <iostream>
#include <glm/glm.hpp>
#include <HelperFunctions.h>
#include <vector>

enum TextureType
{
	ALBEDO = 0,
	METALLIC = 1,
	NORMAL = 2,
	ROUGHNESS = 3,
	AO = 4
};

struct PBRTexture
{
	TextureType type;
	std::string path;

	PBRTexture(std::string path, TextureType textureType)
	{
		this->path = path;
		this->type = textureType;
	}
};

class PBRTextures
{
public:
	unsigned int albedo;
	unsigned int metallic;
	unsigned int roughness;
	unsigned int ao;
	unsigned int normal;

	PBRTextures(std::vector<PBRTexture> textures, Shader shader)
	{
		for (int i = 0; i < textures.size(); i++)
		{
			switch (textures[i].type)
			{
			case ALBEDO:
				this->albedo = loadTexture(textures[i].path.c_str(), true);
			case METALLIC:
				this->metallic = loadTexture(textures[i].path.c_str(), true);
			case ROUGHNESS:
				this->roughness = loadTexture(textures[i].path.c_str(), true);
			case NORMAL:
				this->normal = loadTexture(textures[i].path.c_str(), true);
			case AO:
				this->ao = loadTexture(textures[i].path.c_str(), true);
			default:
				break;
			}
		}

		this->passToShader(shader);
	}

	void useTextures()
	{
		useTexture(ALBEDO, this->albedo);
		useTexture(METALLIC, this->normal);
		useTexture(NORMAL, this->normal);
		useTexture(ROUGHNESS, this->roughness);
		useTexture(AO, this->ao);
	}

private:
	void passToShader(Shader shader)
	{
		shader.use();
		shader.setInt("_albedoMap", ALBEDO);
		shader.setInt("_metallnesMap", METALLIC);
		shader.setInt("_normalMap", NORMAL);
		shader.setInt("_roughnessMap", ROUGHNESS);
		shader.setInt("_aoMap", AO);
	}

};