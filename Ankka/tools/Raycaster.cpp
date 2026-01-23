#include "Ankka/tools/Raycaster.h"

glm::vec3 Raycaster::screenToWorld(
	double xpos, double ypos, float z,
	GLFWwindow* window, glm::mat4 view, glm::mat4 proj)
{

	int width, height;
	glfwGetWindowSize(window, &width, &height);

	glm::vec3 near = glm::unProject(
		glm::vec3((float)xpos, (float)(height - ypos), 0.0f),
		view,
		proj,
		glm::vec4(0,0,width,height));

	glm::vec3 far = glm::unProject(
		glm::vec3((float)xpos, (float)(height - ypos), 1.0f),
		view,
		proj,
		glm::vec4(0, 0, width, height));

	glm::vec3 direction = glm::normalize(far - near);

	float intersection_z = (z - near.z) / direction.z;
	glm::vec3 hit = near + direction * intersection_z;

	return glm::vec3(hit.x, hit.y, hit.z);

}