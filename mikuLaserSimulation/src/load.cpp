#include <iostream>
#include <fstream>
#include <vector>

// GLEW
#define GLEW_STATIC
#include <GL/glew.h>

// GLFW
#include <GLFW/glfw3.h>

//SOIL
#include <SOIL.h>

//glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//user include
#include "Shader.h"
#include "Camera.h"
#include "Mesh.h"
#include "load.h"

void loadGroundMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures) {
	Vertex leftUp, rightUp, leftDown, rightDown;

	leftUp.Position = glm::vec3(-1.0f, 1.0f, 0.0f);
	leftUp.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
	leftUp.TexCoords = glm::vec2(0.0f, 1.0f);

	rightUp.Position = glm::vec3(1.0f, 1.0f, 0.0f);
	rightUp.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
	rightUp.TexCoords = glm::vec2(1.0f, 1.0f);

	leftDown.Position = glm::vec3(-1.0f, -1.0f, 0.0f);
	leftDown.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
	leftDown.TexCoords = glm::vec2(0.0f, 0.0f);

	rightDown.Position = glm::vec3(1.0f, -1.0f, 0.0f);
	rightDown.Normal = glm::vec3(0.0f, 0.0f, 1.0f);
	rightDown.TexCoords = glm::vec2(1.0f, 0.0f);

	vertices.push_back(leftDown);
	vertices.push_back(leftUp);
	vertices.push_back(rightUp);
	vertices.push_back(rightDown);

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);
	indices.push_back(2);
	indices.push_back(3);
	indices.push_back(0);

	Texture t;
	t.path = "resource/ground_texture.jpg";
	t.type = TextureType::Diffuse;
	t.id = loadTextureFromFile("resource/ground_texture.jpg", "");
	textures.push_back(t);
}

GLint loadTextureFromFile(const char* path, std::string directory)
{
	//Generate texture ID and load texture data 
	std::string filename = std::string(path);
	filename = directory + filename;
	GLuint textureID;
	glGenTextures(1, &textureID);
	int width, height;
	unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
	// Assign texture to ID
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	SOIL_free_image_data(image);
	return textureID;
}
