#pragma once
#include <string>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

class Shader
{
public:
	bool loadShaders(std::string vertexShaderFileName, std::string framentShaderFileName);
	void use();
	void cleanup();
	void setM4_Uniform(const std::string& name, glm::mat4 m);

private:
	GLuint mShaderProgram = 0;
	GLuint readShader(const std::string shaderFileName, GLuint shaderType);
};