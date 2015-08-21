#pragma once
#include <vector>
#include <string>
//load function
void loadGroundMesh(std::vector<Vertex> &vertices, std::vector<GLuint> &indices, std::vector<Texture> &textures);
void loadEnvironment(GLuint &vertexBufferObject, GLuint &vertexArrayObject, GLuint &elementBufferObject);
GLint loadTextureFromFile(const char* path, std::string directory);