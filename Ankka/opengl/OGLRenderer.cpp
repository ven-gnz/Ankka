#include "opengl/OGLRenderer.h"
#include "Ankka/Logger.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <ostream>

OGLRenderer::OGLRenderer(GLFWwindow* window) {
	mWindow = window;
	mViewMatrix = glm::mat4(1.0f);
	mProjectionMatrix = glm::mat4(1.0f);
}

void OGLRenderer::handleKeyEvents(int key, int scancode, int action, int mods)
{
	if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
	{
		mUseChangedShader = !mUseChangedShader;
		Logger::log(1, "%s : use changed shader\n", __FUNCTION__);
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

	return true;

}

void OGLRenderer::setSize(unsigned int width, unsigned int height)
{
	mFramebuffer.resize(width, height);
	glViewport(0, 0, width, height);
}

void OGLRenderer::uploadData(OGLMesh vertexData)
{
	mTriangleCount = vertexData.vertices.size();
	mVertexBuffer.uploadData(vertexData);
}


void OGLRenderer::draw()
{
	mFramebuffer.bind();
	glClearColor(0.1f, 0.1f, 0.1f, 0.1f);
	glClearDepth(1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

	float t = glfwGetTime();

	if (mUseChangedShader)
	{	
		mChangedShader.use();
		mViewMatrix = glm::rotate(glm::mat4(1.0f), -t, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	else
	{
		mBasicShader.use();
		mViewMatrix = glm::rotate(glm::mat4(1.0f), t, glm::vec3(0.0f, 0.0f, 1.0f));
	}
	mUniformBuffer.uploadUboData(mViewMatrix, mProjectionMatrix);
	
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount * 3);
	mVertexBuffer.unbind();
	mTex.unbind();
	mFramebuffer.unbind();

	mFramebuffer.drawToScreen();
}

void OGLRenderer::cleanup()
{
	mBasicShader.cleanup();
	mChangedShader.cleanup();

}