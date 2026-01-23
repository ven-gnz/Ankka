#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "opengl/Texture.h"
#include "opengl/OGLRenderData.h"


class GltfModel {
public:
	bool loadModel(OGLRenderData& renderData,
		std::string modelfilename,
		std::string textureFilename);

	void draw();
	void cleanup();
	void uploadVertexBuffers();
	void uploadIndexBuffer();
	glm::mat4& modelMatrix();

private:
	void createVertexBuffers();
	void createIndexBuffer();
	int getTriangleCount();

	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	std::shared_ptr<tinygltf::Model> mModel = nullptr;

	GLuint mVAO = 0;
	std::vector<GLuint> mVertexVBO{};
	GLuint mIndexVBO = 0;

	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL",1}, {"TEXCOORD_0" , 2} };

	Texture mTex{};
	
};