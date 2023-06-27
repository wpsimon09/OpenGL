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

class Model {
	public:
		Model(char* path)
		{
			loadModel(path);
		}
		void Draw(Shader& shader);
	private:
		//model data
		std::vector<Mesh> meshes;
		std::string directory;

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
		//proccess vertex positions, normals and texture coordinates
		glm::vec3 tempVertex;
		tempVertex.x = mesh->mVertices[i].x;
		tempVertex.y = mesh->mVertices[i].y;
		tempVertex.z = mesh->mVertices[i].z;

		vertex.Postion = tempVertex;
	}

	tempprocess indecies

	//process material

	return Mesh(vertecies, indecies, textures);
}

#endif // !MODEL_H
