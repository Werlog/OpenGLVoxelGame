#include "Shader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <glad/glad.h>

Shader::Shader() {
	programHandle = 0;
}

void Shader::loadShader(const std::string& vertexPath, const std::string& fragmentPath) {
	if (programHandle != 0) return;

	std::ifstream vertexFile;
	std::ifstream fragmentFile;
	std::stringstream vShaderStream;
	std::stringstream fShaderStream;
	std::string vertexCode;
	std::string fragmentCode;

	vertexFile.open(RESOURCES_PATH + vertexPath);
	if (vertexFile.is_open()) {
		vShaderStream << vertexFile.rdbuf();
		vertexFile.close();
		vertexCode = vShaderStream.str();
	}
	else return;

	fragmentFile.open(RESOURCES_PATH + fragmentPath);
	if (fragmentFile.is_open()) {
		fShaderStream << fragmentFile.rdbuf();
		fragmentFile.close();
		fragmentCode = fShaderStream.str();
	}
	else return;

	unsigned int vertexHandle = glCreateShader(GL_VERTEX_SHADER);
	const char* cstrVertexCode = vertexCode.c_str();
	glShaderSource(vertexHandle, 1, &cstrVertexCode, NULL);
	glCompileShader(vertexHandle);
	
	int success = 1;
	char log[512];
	glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexHandle, 512, NULL, log);
		std::cout << "VERTEX SHADER COMPILATION FAILED\n" << log << std::endl;
		glDeleteShader(vertexHandle);
		return;
	}

	unsigned int fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);
	const char* cstrFragmentCode = fragmentCode.c_str();
	glShaderSource(fragmentHandle, 1, &cstrFragmentCode, NULL);
	glCompileShader(fragmentHandle);

	glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentHandle, 512, NULL, log);
		std::cout << "FRAGMENT SHADER COMPILATION FAILED\n" << log << std::endl;
		glDeleteShader(vertexHandle);
		glDeleteShader(fragmentHandle);
		return;
	}

	programHandle = glCreateProgram();
	glAttachShader(programHandle, vertexHandle);
	glAttachShader(programHandle, fragmentHandle);

	glLinkProgram(programHandle);

	glGetProgramiv(programHandle, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(programHandle, 512, NULL, log);
		std::cout << "PROGRAM LINKING FAILED\n" << log << std::endl;
	}

	glDeleteShader(vertexHandle);
	glDeleteShader(fragmentHandle);
}