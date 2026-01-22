#pragma once

#include <glm/glm.hpp>
#include <vector>

#include <GLFW/glfw3.h>

struct OGLRenderData
{
	GLFWwindow *rdWindow = nullptr;
	bool rdUseChangedShader = false;
	bool isVSYNC = true;
	int rdFielfOfView = 90;
	unsigned int rdWidth = 0;
	unsigned int rdHeight = 0;
	unsigned int rdTriangelCount = 0;
	unsigned int rdGltfTriangleCount = 0;

	float rdFrameTime = 0.0f;
	float rdUIGenerateTime = 0.0f;

	float rdViewAzimuth = 320.0f;
	float rdViewElevation = -15.0f;

	int rdMoveForward = 0;
	int rdMoveRight = 0;
	int rdMoveUp = 0;

	float rdTickDiff = 0.0f;
	glm::vec3 rdCameraWorldPosition = glm::vec3(-0.5, 1.25, -1.25);
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