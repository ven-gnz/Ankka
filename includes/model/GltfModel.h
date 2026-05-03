#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "opengl/Texture.h"
#include "opengl/OGLRenderData.h"
#include "opengl/Shader.h"
#include "model/GltfNode.h"
#include "model/ModelLoader.h"
#include "model/MeshPrimitive.h"
#include "model/GltfAnimationClip.h"

class GltfModel {
public:
	bool loadModel(OGLRenderData& renderData,
		std::string modelfilename,
		std::string textureFilename);

	void draw(Shader s);
	void cleanup();
	void uploadVertexBuffers();
	void uploadIndexBuffer();
	void uploadPositionBuffer();
	glm::mat4& modelMatrix();

	std::shared_ptr<OGLMesh> getSkeleton(bool enableSkinning);
	void applyCPUVertexSkinning();
	std::vector<glm::mat4> getJointMatrices();
	std::vector<glm::mat2x4> getJointDualQuats();
	int getJointDualQuatsSize(); 

	int getJointMatrixSize();
	bool mSkinned;

	glm::vec3 calculateAABB(const tinygltf::Accessor& accessor,
		const tinygltf::BufferView& bufferView,
		const tinygltf::Buffer& buffer,
		glm::vec3& maxi);

	void playAnimation(int animNum, float speedDivider);
	void setAnimationFrame(int animNumber, float time);
	float getAnimationEndTime(int animNum);
	std::string getClipName(int animNum);

private:
	void createVertexBuffers();
	void createIndexBuffer();
	int getTriangleCount();
	ModelLoader mModelLoader{};

	void getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning);

	void getJointData();
	void getWeightData();
	void getInvBindMatrices();
	void getNodes(std::shared_ptr<GltfNode> treeNode);
	void getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix);
	void drawNode(std::shared_ptr<GltfNode> node, glm::mat4 parentMatrix, Shader s);

	std::vector < glm::mat2x4> mJointDualQuats{};
	std::vector<glm::tvec4<uint16_t>> mJointVec{};
	std::vector<glm::vec4> mWeightVec{};
	std::vector<glm::mat4> mInverseBindMatrices{};
	std::vector<glm::mat4> mJointMatrices{};

	std::vector<int> mAttribAccessors{};
	std::vector<int> mNodeToJoint{};
	
	std::vector<glm::vec3> mAlteredPositions{};
	std::vector<std::shared_ptr<GltfNode>> mNodeList{};
	void updateNodesMatrices(
		std::shared_ptr<GltfNode> node,
		glm::mat4 parentNodeMatrix);
	void updateJointMatricesAndQuats(std::shared_ptr<GltfNode> treeNode);

	std::vector<glm::vec3> mNormals{};
	void calculateNormals(std::string modelFileName);

	glm::mat4 mModelMatrix = glm::mat4(1.0f);
	std::shared_ptr<tinygltf::Model> mModel = nullptr;

	std::shared_ptr<GltfNode> mRootNode = nullptr;
	
	std::shared_ptr<OGLMesh> mSkeletonMesh = nullptr;

	std::vector<GltfAnimationClip> mAnimClips{};
	void getAnimations();
	

	GLuint mVAO = 0;
	std::vector<GLuint> mVertexVBO{};
	GLuint mIndexVBO = 0;
	std::string mTexNameStr;

	std::map<std::string, GLint> attributes = {
		{"POSITION", 0},
		{"NORMAL", 1 },
		{"TEXCOORD_0" , 2},
		{"JOINTS_0" , 3},
		{"WEIGHTS_0", 4} };

	int mVertexCount;

	glm::vec3 mLocalAABBmin;
	glm::vec3 mLocalAABBmax;

	Texture mTex{};
	
};