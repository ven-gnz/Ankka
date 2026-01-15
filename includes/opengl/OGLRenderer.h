#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "opengl/Framebuffer.h"
#include "opengl/VertexBuffer.h"
#include "opengl/Texture.h"
#include "opengl/Shader.h"
#include "opengl/UniformBuffer.h"


#include "imgui_impl_opengl3.h"
#include "imgui_impl_glfw.h"

#include "OGLRenderData.h"

class OGLRenderer
{
public:
	OGLRenderer(GLFWwindow* win);
	bool init(unsigned int width, unsigned int height);
	void setSize(unsigned int width, unsigned int height);
	void cleanup();
	void uploadData(OGLMesh vertexData);
	void draw();
	void handleKeyEvents(int key, int scancode, int action, int mods);

private:

	Shader mBasicShader{};
	Shader mChangedShader{};
	Framebuffer mFramebuffer{};
	VertexBuffer mVertexBuffer{};
	UniformBuffer mUniformBuffer;
	Texture mTex{};
	bool mUseChangedShader = false;


	OGLRenderData mRenderData{};

	glm::vec3 cameraPosition;
	glm::vec3 cameraLookAtPosition;
	glm::vec3 cameraUpVector;


	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

};