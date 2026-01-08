#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl/Framebuffer.h"
#include "opengl/VertexBuffer.h"
#include "opengl/Texture.h"
#include "opengl/Shader.h"

#include "OGLRenderData.h"

class OGLRenderer
{
public:
	bool init(unsigned int width, unsigned int height);
	void setSize(unsigned int width, unsigned int height);
	void cleanup();
	void uploadData(OGLMesh vertexData);
	void draw();

private:
	Shader mShader{};
	Framebuffer mFramebuffer{};
	VertexBuffer mVertexBuffer{};
	Texture mTex{};
	int mTriangleCount = 0;

};