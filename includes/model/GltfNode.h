#pragma once
#include <vector>
#include <memory>
#include <string>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include<algorithm>
#include <glm/gtx/matrix_decompose.hpp>

class GltfNode : public std::enable_shared_from_this<GltfNode> {

	public:
	static std::shared_ptr<GltfNode> createRoot(int rootNodeNum);
	std::shared_ptr<GltfNode> getParentNode();
	void addChilds(std::vector<int> childNodes);
	std::vector<std::shared_ptr<GltfNode>> getChilds();
	int getNodeNum();

	void setNodeName(std::string name);
	void setScale(glm::vec3 scale);
	void setTranslation(glm::vec3 translation);
	void setRotation(glm::quat rotation);

	void calculateLocalTRSMatrix();
	void calculateNodeMatrix();
	glm::mat4 getNodeMatrix();

	void printTree();

	void blendScale(glm::vec3 scale, float blendFactor);
	void blendTranslation(glm::vec3 translation, float blendFactor);
	void blendRotation(glm::quat rotation, float blendFactor);
	std::string getNodeName();

	glm::quat getLocalRotation();
	glm::quat getGlobalRotation();
	glm::vec3 getGlobalPosition();

	void setWorldPosition(glm::vec3 pos);
	glm::vec3 getWorldPosition();
	void setWorldRotation(glm::vec3 rot);

	void updateNodeAndChildMatrices();

private:

	glm::vec3 mBlendScale = glm::vec3(1.0f);
	glm::vec3 mBlendTranslation = glm::vec3(0.0f);
	glm::quat mBlendRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	glm::vec3 mWorldPosition = glm::vec3(0.0f);
	glm::vec3 mWorldRotation = glm::vec3(0.0f);

	int mNodeNum = 0;
	std::string mNodeName;

	std::vector<std::shared_ptr<GltfNode>> mChildNodes{};
	glm::vec3 mScale = glm::vec3(1.0f);
	glm::vec3 mTranslation = glm::vec3(0.0f);
	glm::quat mRotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

	glm::mat4 mLocalTRSMatrix = glm::mat4(1.0f);
	glm::mat4 mNodeMatrix = glm::mat4(1.0f);
	std::weak_ptr<GltfNode> mParentNode;

	//OPTIMIZATIONS : promoted to member variables for less loadop on claculate localTRSMatrix
	glm::mat4 mTranslationMatrix = glm::mat4(1.0f);
	glm::mat4 mRotationMatrix = glm::mat4(1.0f);
	glm::mat4 mScaleMatrix = glm::mat4(1.0f);


	glm::mat4 mWorldTranslationMatrix = glm::mat4(1.0f);
	glm::mat4 mWorldRotationMatrix = glm::mat4(1.0f);
	glm::mat4 mWorldTRMatrix = glm::mat4(1.0f);

	bool mLocalMatrixNeedsUpdate = true;
	

	void printNodes(std::shared_ptr<GltfNode> startNode, int indent);
};