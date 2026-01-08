#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shader
{
public:
	bool loadShaders(std::string vertexShaderFileName, std::string framentShaderFileName);
	void use();
	void cleanup();

private:
	GLuint mShaderProgram = 0;
	GLuint readShader(std::string shaderFileName, GLuint shaderType);
};