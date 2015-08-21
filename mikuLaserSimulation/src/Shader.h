#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include <GL/glew.h>
class Shader
{
public:
	GLuint program;
	Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath);
	Shader(const GLchar *vertexSourcePath, const GLchar *geometrySourcePath,const GLchar *fragmentSourcePath);
	~Shader();
	void use();
private:
	std::string loadShaderCString(const GLchar * fileName);
};
#endif
