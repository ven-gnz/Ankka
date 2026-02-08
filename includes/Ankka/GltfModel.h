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

	std::shared_ptr<OGLMesh> getSkeleton(bool enableSkinning);

private:
	void createVertexBuffers();
	void createIndexBuffer();
	int getTriangleCount();

	void getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning);

	void getJointData();
	void getWeightData();
	void getInvBindMatrices();
	void getNodes(std::shared_ptr<GltfNode> treeNode);
	void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);

	std::vector<glm::tvec4<uint16_t>> mJointVec{};
	std::vector<glm::vec4> mWeightVec{};
	std::vector<glm::mat4> mInverseBindMatrices{};
	std::vector<glm::mat4> mJointMatrices{};

	std::vector<int> mAttribAccessors{};
	std::vector<int> mNodeToJoint{};
	
	std::vector<glm::vec3> mAlteredPositions{};


	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	std::shared_ptr<tinygltf::Model> mModel = nullptr;

	std::shared_ptr<GltfNode> mRootNode = nullptr;
	
	std::shared_ptr<OGLMesh> mSkeletonMesh = nullptr;
	

	GLuint mVAO = 0;
	std::vector<GLuint> mVertexVBO{};
	GLuint mIndexVBO = 0;

	std::map<std::string, GLint> attributes = { {"POSITION", 0}, {"NORMAL",1}, {"TEXCOORD_0" , 2} };

	int mVertexCount;

	glm::vec3 mLocalAABBmin;
	glm::vec3 mLocalAABBmax;

	Texture mTex{};
	
};