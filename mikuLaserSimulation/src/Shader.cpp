#include "Shader.h"



Shader::Shader(const GLchar *vertexSourcePath, const GLchar *fragmentSourcePath)
{
	//1.读取着色器代码
	std::string vertexCode=loadShaderCString(vertexSourcePath);
	std::string fragmentCode=loadShaderCString(fragmentSourcePath);
	const char *vertexShaderSource_c_str = vertexCode.c_str();
	const char *fragmentShaderSource_c_str = fragmentCode.c_str();

	//2.编译着色器
	GLuint vertex, fragment;
	GLint vertexSuccess,fragmentSuccess;
	GLchar vertexInfoLog[512];
	GLchar fragmentInfoLog[512];

	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex,1,&vertexShaderSource_c_str,NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &vertexSuccess);//判断编译是否有错误
	if (!vertexSuccess) {
		glGetShaderInfoLog(vertex, 512, NULL, vertexInfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexInfoLog << std::endl;
	}

	//片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment,1,&fragmentShaderSource_c_str,NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &fragmentSuccess);//判断编译是否有错误
	if (!fragmentSuccess) {
		glGetShaderInfoLog(fragment, 512, NULL, fragmentInfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentInfoLog << std::endl;
	}

	//3.连接着色器
	GLint linkSuccess;
	GLchar linkInfoLog[512];
	this->program = glCreateProgram();
	glAttachShader(this->program,vertex);
	glAttachShader(this->program,fragment);
	glLinkProgram(this->program);
	glGetProgramiv(this->program,GL_LINK_STATUS,&linkSuccess);
	if (!linkSuccess) {
		glGetProgramInfoLog(this->program,512,NULL,linkInfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" <<linkInfoLog<< std::endl;
	}

	//4.删除着色器
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}


Shader::Shader(const GLchar *vertexSourcePath, const GLchar *geometrySourcePath, const GLchar *fragmentSourcePath)
{
	//1.读取着色器代码
	std::string vertexCode = loadShaderCString(vertexSourcePath);
	std::string fragmentCode = loadShaderCString(fragmentSourcePath);
	std::string geometryCode = loadShaderCString(geometrySourcePath);
	const char *vertexShaderSource_c_str = vertexCode.c_str();
	const char *fragmentShaderSource_c_str = fragmentCode.c_str();
	const char *geometryShaderSource_c_str = geometryCode.c_str();

	//2.编译着色器
	GLuint vertex, fragment, geometry;
	GLint vertexSuccess, fragmentSuccess, geometrySuccess;
	GLchar vertexInfoLog[512];
	GLchar fragmentInfoLog[512];
	GLchar geometryInfoLog[512];

	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vertexShaderSource_c_str, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &vertexSuccess);//判断编译是否有错误
	if (!vertexSuccess) {
		glGetShaderInfoLog(vertex, 512, NULL, vertexInfoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << vertexInfoLog << std::endl;
	}

	//几何着色器
	geometry = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(geometry, 1, &geometryShaderSource_c_str, NULL);
	glCompileShader(geometry);
	glGetShaderiv(geometry, GL_COMPILE_STATUS, &geometrySuccess);//判断编译是否有错误
	if (!geometrySuccess) {
		glGetShaderInfoLog(geometry, 512, NULL, geometryInfoLog);
		std::cout << "ERROR::SHADER::GEOMETRY::COMPILATION_FAILED\n" << geometryInfoLog << std::endl;
	}

	//片段着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fragmentShaderSource_c_str, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &fragmentSuccess);//判断编译是否有错误
	if (!fragmentSuccess) {
		glGetShaderInfoLog(fragment, 512, NULL, fragmentInfoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << fragmentInfoLog << std::endl;
	}




	

	//3.连接着色器
	GLint linkSuccess;
	GLchar linkInfoLog[512];
	this->program = glCreateProgram();
	glAttachShader(this->program, vertex);
	glAttachShader(this->program, geometry);
	glAttachShader(this->program, fragment);
	glLinkProgram(this->program);
	glGetProgramiv(this->program, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess) {
		glGetProgramInfoLog(this->program, 512, NULL, linkInfoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n" << linkInfoLog << std::endl;
	}

	//4.删除着色器
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	glDeleteShader(geometry);
}

void Shader::use()
{
	glUseProgram(this->program);
}
Shader::~Shader()
{
}

std::string Shader::loadShaderCString(const GLchar * fileName)
{

	std::ifstream infile;
	infile.open(fileName);
	if (!infile) {
		std::cout << "open shader file "<< fileName<<" failed!" << std::endl;
		return "";
	}
	std::string result((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());
	infile.close();
	return result;
}
