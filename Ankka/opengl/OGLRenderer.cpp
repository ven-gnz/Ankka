#include "OGLRenderer.h"

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

	if (!mTex.loadTexture("textures/crate.png")) {
		return false;
	}

	mVertexBuffer.init();

	if (!mShader.loadShader("shader/basic.vert", "shader/basic.frag"))
	{
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_CULL_FACE);

	mShader.use();
	mTex.bind();
	mVertexBuffer.bind();
	mVertexBuffer.draw(GL_TRIANGLES, 0, mTriangleCount);
	mVertexBuffer.unbind();
	mTex.unbind();
	mFramebuffer.unbind();

	mFramebuffer.drawToScreen();
}