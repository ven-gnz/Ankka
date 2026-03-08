#include <opengl/ShaderStorageBuffer.h>
#include <Ankka/logger.h>


void ShaderStorageBuffer::init(size_t bufferSize)
{
	mBufferSize = bufferSize;

	glGenBuffers(1, &mShaderStorageBuffer);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mShaderStorageBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, mBufferSize, NULL, GL_DYNAMIC_COPY);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::uploadSsboData(
	std::vector<glm::mat4> bufferData, int bindingPoint)
{
	if (bufferData.size() == 0)
	{
		Logger::log(1, "%s: trying to upload an empty ssbo buffer s\n",
			__FUNCTION__);
		return;
	}
	size_t bufferSize = bufferData.size() * sizeof(glm::mat4);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mShaderStorageBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, bufferData.data());
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
		bindingPoint,
		mShaderStorageBuffer,
		0,
		bufferSize);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::uploadSsboData(const std::vector<glm::mat2x4>& bufferData, int bindingPoint)
{

	size_t bufferSize = bufferData.size() * sizeof(glm::mat2x4);
	if (bufferData.size() == 0)
	{
		Logger::log(1, "%s: trying to upload an empty ssbo buffer s\n",
			__FUNCTION__);
		return;
	}

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, mShaderStorageBuffer);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, bufferSize, bufferData.data());
	glBindBufferRange(GL_SHADER_STORAGE_BUFFER,
		bindingPoint,
		mShaderStorageBuffer,
		0,
		bufferSize);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void ShaderStorageBuffer::cleanup()
{
	glDeleteBuffers(1, &mShaderStorageBuffer);
}