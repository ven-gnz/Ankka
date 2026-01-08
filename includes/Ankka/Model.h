#pragma once
#include <vector>
#include <glm/glm.hpp>
#include "opengl/OGLRenderData.h"

class Model
{
public:
	void init();
	OGLMesh getVertexData();

private:
	OGLMesh mVertexData{};
};