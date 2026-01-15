#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <GLFW/glfw3.h>

struct OGLRenderData
{
	GLFWwindow *rdWindow = nullptr;
	unsigned int rdWidth = 0;
	unsigned int rdHeight = 0;
	unsigned int rdTriangelCount = 0;
};

struct OGLVertex
{
	glm::vec3 position;
	glm::vec3 color;
	glm::vec2 uv;
};

struct OGLMesh
{
	std::vector<OGLVertex> vertices;
};