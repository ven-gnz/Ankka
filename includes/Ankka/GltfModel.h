#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "opengl/Texture.h"
#include "opengl/OGLRenderData.h"
#include "Ankka/GltfNode.h"


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

	std::shared_ptr<GltfNode> mRootNode = nullptr;
	void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);
	void getNodes(std::shared_ptr<GltfNode> treeNode);

	GLuint mVAO = 0;
	std::vector<GLuint> mVertexVBO{};
	GLuint mIndexVBO = 0;

	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL",1}, {"TEXCOORD_0" , 2} };

	int mVertexCount;

	glm::vec3 mLocalAABBmin;
	glm::vec3 mLocalAABBmax;

	Texture mTex{};
	
};