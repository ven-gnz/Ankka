#include "opengl/OGLRenderer.h"
#include "Ankka/Logger.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <ostream>

void OGLRenderer::handleMouseButtonEvents(int button, int action, int mods)
{
	ImGuiIO& io = ImGui::GetIO();
	if (button >= 0 && button < ImGuiMouseButton_COUNT)
	{
		io.AddMouseButtonEvent(button, action == GLFW_PRESS);
	}
	if (io.WantCaptureMouse) return;

	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	{
		mMouseLock = !mMouseLock;
	}

	if (mMouseLock) glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	else glfwSetInputMode(mRenderData.rdWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	
}

void OGLRenderer::handleMousePositionEvents(double xPos, double yPos)
{
	ImGuiIO& io = ImGui::GetIO();
	io.AddMousePosEvent((float)xPos, (float)yPos);
	if (io.WantCaptureMouse) return;

	int mouseMoveRelX = static_cast<int>(xPos) - mMouseXPos;
	int mouseMoveRelY = static_cast<int>(yPos) - mMouseYPos;

	if (mMouseLock)
	{
		mRenderData.rdViewAzimuth += mouseMoveRelX / 10.0f;
		if (mRenderData.rdViewAzimuth < 0.0) mRenderData.rdViewAzimuth += 360.0;
		if (mRenderData.rdViewAzimuth >= 360.0) mRenderData.rdViewAzimuth -= 360.0;

		mRenderData.rdViewElevation -= mouseMoveRelY / 10.0;
		if (mRenderData.rdViewElevation > 89.0) mRenderData.rdViewElevation = 89.0;
		if (mRenderData.rdViewElevation < -89.0) mRenderData.rdViewElevation = -89.0;
	}

	mMouseXPos = static_cast<int>(xPos);
	mMouseYPos = static_cast<int>(yPos);

}


void OGLRenderer::toggleVsync()
{
	int bool_Vsync = mRenderData.isVSYNC ? 1 : 0;
	glfwSwapInterval(bool_Vsync);
	old_VSync = bool_Vsync;
}

OGLRenderer::OGLRenderer(GLFWwindow* window) {

	mRenderData.rdWindow = window;
	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);

	cameraPosition = glm::vec3(0.4f, 0.3f, 1.0f);
	cameraLookAtPosition = glm::vec3(0.0f, 0.0f, 0.0f);
	cameraUpVector = glm::vec3(0.0f, 1.0f, 0.0f);
}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	if (glfwGetKey(mRenderData.rdWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mRenderData.rdUseChangedShader = !mRenderData.rdUseChangedShader;
		Logger::log(1, "using shader : %s\n",
			mRenderData.rdUseChangedShader ? "changed" : "normal",
			__FUNCTION__);
	}
}

bool OGLRenderer::init(unsigned int width, unsigned int height)
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		return false;
	}
	if (!GLAD_GL_VERSION_4_6) { return false; }

	if (!mFramebuffer.init(width, height))
	{
		return false;
	}

	
	if (!mTex.loadTexture("tex/crate.png")) {
		Logger::log(1, "%s: cannot find texture", __FUNCTION__);
		return false;
	}

	mVertexBuffer.init();

	if (!mBasicShader.loadShaders("shaders/basic.vert", "shaders/basic.frag"))
	{
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

	if (!mChangedShader.loadShaders("shaders/changed.vert", "shaders/changed.frag"))
	{
		Logger::log(1, "% s: cannot find shaders\n", __FUNCTION__);
		return false;
	}

	mUniformBuffer.init();
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	mRenderData.rdWidth = width;
	mRenderData.rdHeight = height;

	mUserInterface.init(mRenderData);

	return true;

}

void OGLRenderer::setSize(unsigned int width, unsigned int height)
{
	mFramebuffer.resize(width, height);
	glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData)
{
	mRenderData.rdTriangelCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
}


void OGLRenderer::draw()
{
	static float prevFrameStartTime = 0.0f;
	float frameStartTime = glfwGetTime();

	if (mRenderData.isVSYNC != old_VSync)
	{
		toggleVsync();
	}

	mFramebuffer.bind();
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	mProjectionMatrix = glm::perspective(
		static_cast<float>(mRenderData.rdFielfOfView),
		static_cast<float>(mRenderData.rdWidth) / static_cast<float>(mRenderData.rdHeight),
		0.01f,
		100.0f);

	float t = glfwGetTime();

	//glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 model = glm::mat4(1.0f);
	
	if (mRenderData.rdUseChangedShader)
	{	
		mChangedShader.use();
		model = glm::rotate(glm::mat4(1.0f), -t, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		mBasicShader.use();
		model = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	mViewMatrix = mCamera.getViewMatrix(mRenderData) * model;
	mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);
	
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mRenderData.rdTriangelCount * 3);
	mVertexBuffer.unbind();
	mTex.unbind();
	mFramebuffer.unbind();

	mFramebuffer.drawToScreen();
	mUIGenerateTimer.start();
	mUserInterface.createFrame(mRenderData);
	mUserInterface.render();
	mRenderData.rdUIGenerateTime = mUIGenerateTimer.stop();
	mRenderData.rdFrameTime = frameStartTime - prevFrameStartTime;
	prevFrameStartTime = frameStartTime;
}

void OGLRenderer::cleanup()
{
	mUserInterface.cleanup();
	mBasicShader.cleanup();
	mChangedShader.cleanup();

}