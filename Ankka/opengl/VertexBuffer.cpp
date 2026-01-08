#include "opengl/VertexBuffer.h"


void VertexBuffer::init()
{
	glGenVertexArrays(1, &mVAO);
	glGenBuffers(1, &mVertexVBO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(OGLVertex), (void*)offsetof(OGLVertex, uv));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
}

void VertexBuffer::cleanup()
{
	glDeleteBuffers(1, &mVertexVBO);
	glDeleteVertexArrays(1, &mVAO);
}

void VertexBuffer::uploadData(OGLMesh vertexData)
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO);
	glBufferData(GL_ARRAY_BUFFER, vertexData.vertices.size() * sizeof(OGLVertex), &vertexData.vertices.at(0), GL_DYNAMIC_DRAW);

	glBindVertexArray(0);
}

void VertexBuffer::bind() { glBindVertexArray(mVAO); }
void VertexBuffer::unbind() { glBindVertexArray(0); }

void VertexBuffer::draw(GLuint mode, unsigned int start, unsigned int num)
{
	glDrawArrays(mode, start, num);
}