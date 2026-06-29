#pragma once
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <glfw/glfw3.h>
#include <glm/ext/matrix_projection.hpp>

class Raycaster
{
public:
	glm::vec3 screenToWorld(double xPos, double ypos, float z,
		GLFWwindow* window,
		glm::mat4 view,
		glm::mat4 projection);
};