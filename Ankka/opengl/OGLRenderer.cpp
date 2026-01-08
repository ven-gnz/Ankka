#include "opengl/OGLRenderer.h"
#include "Ankka/Logger.h"
#include <iostream>
#include <filesystem>
#include <string>
#include <ostream>

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

	if (!mShader.loadShaders("shaders/basic.vert", "shaders/basic.frag"))
	{
		Logger::log(1, "%s: cannot find shaders\n",
			__FUNCTION__);
		return false;
	}

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
	mShader.use();
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount * 3);
	mVertexBuffer.unbind();
	mTex.unbind();
	mFramebuffer.unbind();

	mFramebuffer.drawToScreen();
}