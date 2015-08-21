#include <iostream>
#include <fstream>


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

void loadGroundMesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures) {
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

GLint loadTextureFromFile(const char* path, string directory)
{
	//Generate texture ID and load texture data 
	string filename = string(path);
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

void loadLaser(GLuint &vertexBufferObject, GLuint &vertexArrayObject, GLuint &elementBufferObject,
	vector<glm::vec3> &originPoints, vector<glm::vec3> &endPoints) {
	glGenVertexArrays(1, &vertexArrayObject);
	glGenBuffers(1, &vertexBufferObject);
	glGenBuffers(1, &elementBufferObject);

	//将vector转化为动态数组
	GLfloat *laserVertices = new GLfloat[originPoints.size() * 6];
	GLuint  *laserIndices = new GLuint[originPoints.size() * 2];
	vector<glm::vec3>::iterator first = originPoints.begin();

	int index = 0;
	while (originPoints.end() != first) {
		laserVertices[index] = (*first).x;
		laserVertices[index + 1] = (*first).y;
		laserVertices[index + 2] = (*first).z;
		++first;
		index += 6;
	}
	first = endPoints.begin();
	index = 0;
	while (endPoints.end() != first) {
		laserVertices[index + 3] = (*first).x;
		laserVertices[index + 4] = (*first).y;
		laserVertices[index + 5] = (*first).z;
		++first;
		index += 6;
	}
	for (GLuint i = 0; i < originPoints.size() * 2; ++i) {
		laserIndices[i] = i;
	}
	// Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).
	glBindVertexArray(vertexArrayObject);

	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, sizeof(laserVertices)*sizeof(GLfloat), laserVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(laserIndices)*sizeof(GLuint), laserIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0); // Note that this is allowed, the call to glVertexAttribPointer registered VBO as the currently bound vertex buffer object so afterwards we can safely unbind

	glBindVertexArray(0); // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
	delete[] laserVertices;
	delete[] laserIndices;
}