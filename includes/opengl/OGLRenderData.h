#pragma once

#include <glm/glm.hpp>
#include <vector>

struct OGLVertex
{
	glm::vec3 position;
	glm::vec2 uv;
};

struct OGLMesh
{
	std::vector<OGLVertex> vertices;
};