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

#include "Ankka/UserInterface.h"

#include "Ankka/tools/Timer.h"
#include "Ankka/tools/Camera.h"

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

	void handleMouseButtonEvents(int button, int action, int mods);
	void handleMousePositionEvents(double xPos, double yPos);

	void toggleVsync();

private:

	Camera mCamera{};

	bool mMouseLock = false;
	int mMouseXPos = 0;
	int mMouseYPos = 0;

	double lastTickTime = 0.0;

	void handleMovementKeys();

	Shader mBasicShader{};
	Shader mChangedShader{};
	Framebuffer mFramebuffer{};
	VertexBuffer mVertexBuffer{};
	UniformBuffer mUniformBuffer;
	Texture mTex{};
	bool old_VSync = true;

	OGLRenderData mRenderData{};

	glm::vec3 cameraPosition;
	glm::vec3 cameraLookAtPosition;
	glm::vec3 cameraUpVector;


	glm::mat4 mViewMatrix;
	glm::mat4 mProjectionMatrix;

	UserInterface mUserInterface{};

	Timer mUIGenerateTimer{};

};