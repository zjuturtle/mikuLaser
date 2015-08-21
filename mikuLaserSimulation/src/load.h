#pragma once
//load function
void loadGroundMesh(vector<Vertex> &vertices, vector<GLuint> &indices, vector<Texture> &textures);
void loadEnvironment(GLuint &vertexBufferObject, GLuint &vertexArrayObject, GLuint &elementBufferObject);
void loadLaser(GLuint &vertexBufferObject, GLuint &vertexArrayObject, GLuint &elementBufferObject,
	vector<glm::vec3> &originPoints, vector<glm::vec3> &endPoints);
GLint loadTextureFromFile(const char* path, string directory);