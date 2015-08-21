#pragma once
//load function
void loadGroundMesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures);
void loadEnvironment(GLuint &vertexBufferObject, GLuint &vertexArrayObject, GLuint &elementBufferObject);
GLint loadTextureFromFile(const char* path, string directory);