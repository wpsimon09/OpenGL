#ifndef MODEL_H
#define MODEL_H
#include<glad/glad.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<stb_image.h>
#include<assimp/Importer.hpp>
#include<assimp/postprocess.h>
#include<assimp/ai_assert.h>
#include<assimp/scene.h>

#include "Mesh.h"
#include "Shader.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);


class Model {
	public:
		Model(const char* path)
		{
			loadModel(path);
		}
		void Draw(Shader& shader);
	private:
		//model data
		std::vector<Mesh> meshes;
		std::string directory;
		std::vector<Texture> textures_loaded;

		void loadModel(std::string path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};

/*
* Loops through all of the meshes and draw them using their own 
* draw method
*/
void Model::Draw(Shader& shader) {
	for (int i = 0; i < meshes.size(); i++)
	{
		meshes[i].Draw(shader);
	}
}


/*
* Loads model using Assimp importer class
*/
void Model::loadModel(std::string path) {
	Assimp::Importer importer;
	
	/*
	* Loads all of the file data to the sceneobject
	* 
	* @param: path - path to the object file
	* @param: postproccesing arguments - postprocessing arguments
	* 
	* aiProcess_triangles - assimp convert everything to triagles
	* aiProcess_flipUvS - assimp will flip textures accordingly
	* 
	*/
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
		std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
		return;
	}
	this->directory = path.substr(0, path.find_last_of('/'));
	processNode(scene->mRootNode, scene);
	std::cout << "-------------------MODEL PROCESSED------------------------\n\n\n";

}


void Model::processNode(aiNode* node, const aiScene* scene) {
	
	// process all nodes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		//scene has multiple nodes and node indexes mesh
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	//process node's schildren
	for (unsigned int  i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
	std::vector<Vertex> vertecies;
	std::vector<unsigned int> indecies;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		//proccess vertex positions
		glm::vec3 tempVertex;
		tempVertex.x = mesh->mVertices[i].x;
		tempVertex.y = mesh->mVertices[i].y;
		tempVertex.z = mesh->mVertices[i].z;
		vertex.Postion = tempVertex;
		
		if (mesh->HasNormals())
		{

			tempVertex.x = mesh->mNormals[i].x;
			tempVertex.y = mesh->mNormals[i].y;
			tempVertex.z = mesh->mNormals[i].z;
			
			vertex.Normal = tempVertex;
			
		}
		else 
			std::cout << "ERROR::ASSIMP::PROCCESSING::NORMALS - There are no normal vectors in the model\n";
	
		//process normal vectors
		//process texture coordinates
		glm::vec2 tempTexCoords;

		if (mesh->mTextureCoords[0])
		{			
			//asimp allows for 8 different texture coorinates per verex
			//but we are only interted in the first one 
			tempTexCoords.x = mesh->mTextureCoords[0][i].x;
			tempTexCoords.y = mesh->mTextureCoords[0][i].y;
			vertex.TexCoords = tempTexCoords;

		}
		else {
			std::cout << "ERROR::ASSIMP::PROCCESSING::TEXTURE::COORDINATES - There are no texture coordinates to process\n";
			vertex.TexCoords = glm::vec2(0.0f, 0.0f);
		}
		vertecies.push_back(vertex);
	}
	
	//process indecies
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		//each mesh contains n number of faces
		//each face contains mnumber of indecies 
		for (unsigned int j = 0; j < face.mNumIndices; j++)
		{
			indecies.push_back(face.mIndices[j]);
		}
	}
	
	//process material
	if (mesh->mMaterialIndex >0)
	{
		// load diffuse textures
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
		
		std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		//insert on the end of texture vector
		//from diffuse maps begin to diffuse map end
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		// load specular textures
		std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");

		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		// load emision textures
		std::vector<Texture> emisionMaps = loadMaterialTextures(material, aiTextureType_EMISSION_COLOR, "texture_emmision");
	}
	else 
		std::cout << "ERROR::ASSIMP::PROCCESSING::MATERIALS - There are no materials to be proccessed\n";

	std::cout << "--------------------------------------------------------------\n";
	std::cout << "--------------------MESH LOADING DONE-------------------------\n";
	std::cout << "-------------------VERTECIES PROCESSED------------------------\n";
	std::cout << "-------------------INDECIES PROCESSED-------------------------\n";
	std::cout << "-------------------MATERIALS PROCESSED------------------------\n";
	std::cout << "--------------------------------------------------------------\n\n\n";
	return Mesh(vertecies, indecies, textures);
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(),str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true;
				break;
			}
		}
		if (!skip)
		{ // if texture hasn�t been loaded already, load it
			Texture texture;
			texture.id = TextureFromFile(str.C_Str(), this->directory);
			texture.type = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture); // add to loaded textures
		}
	}

	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

#endif // !MODEL_H
