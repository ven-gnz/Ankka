#include <chrono>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <cstdlib>

#include <model/GltfInstance.h>
#include <Ankka/Logger.h>

GltfInstance::~GltfInstance()
{
	
}

GltfInstance::GltfInstance(std::shared_ptr<GltfModel> model, glm::vec2 worldPos, bool randomize)
{
	if (!model)
	{
		Logger::log(1, "%s error : invalid glTF model! \n", __FUNCTION__);
		return;
	}

	mGltfModel = model;
	mModelSettings.msWorldPosition = worldPos;
	mNodeCount = mGltfModel->getNodeCount();

	mInverseBindMatrices = mGltfModel->getInverseBindMatrices();
	mNodeToJoint = mGltfModel->getNodeToJoint();

	mJointMatrices.resize(mInverseBindMatrices.size());
	mJointDualQuats.resize(mInverseBindMatrices.size());

	mAdditiveAnimationMask.resize(mNodeCount);
	mInvertedAdditiveAnimationMask.resize(mNodeCount);

	std::fill(mAdditiveAnimationMask.begin(), mAdditiveAnimationMask.end(), true);
	mInvertedAdditiveAnimationMask = mAdditiveAnimationMask;
	mInvertedAdditiveAnimationMask.flip();

	GltfNodeData nodeData;
	nodeData = mGltfModel->getGltfNodes();
	mRootNode = nodeData.rootNode;
	mRootNode->setWorldPosition(glm::vec3(mModelSettings.msWorldPosition.x, 0.0f,
		mModelSettings.msWorldPosition.y));

	mNodeList = nodeData.nodeList;

	mModelSettings.msSkelSplitNode = mNodeCount - 1;

	for (const auto& node : mNodeList) {
		if (node) {
			mModelSettings.msSkelNodeNames.push_back(node->getNodeName());
		}else {
			mModelSettings.msSkelNodeNames.push_back("invalid");
		}
	}

	updateNodeMatrices(mRootNode);

	mAnimClips = mGltfModel->getAnimClips();
	for (const auto& clip : mAnimClips)
	{
		mModelSettings.msClipNames.push_back(clip->getClipName());
	}
	unsigned int animClipSize = mAnimClips.size();

	if (randomize) {
		int animClip = std::rand() % animClipSize;
		float animClipSpeed = (std::rand() % 100) / 100.0f + 0.5f;
		float initRotation = std::rand() % 360 - 180;
		mModelSettings.msAnimClip = animClip;
		mModelSettings.msAnimSpeed = animClipSpeed;
		mModelSettings.msWorldRotation = glm::vec3(0.0f, initRotation, 0.0f);
		mRootNode->setWorldRotation(mModelSettings.msWorldRotation);
	}

	checkForUpdates();

	mSkeletonMesh = std::make_shared<OGLMesh>();
	mSkeletonMesh->vertices.resize(mNodeCount * 2);
	
	/* set values for inverse kinematics */
	/* hard-code right arm here for startup as in the example*/
	mModelSettings.msIkEffectorNode = 19;
	mModelSettings.msIkRootNode = 26;
	setInverseKinematicsNodes(mModelSettings.msIkEffectorNode, mModelSettings.msIkRootNode);
	setNumIKIterations(mModelSettings.msIkIterations);

	mModelSettings.msIkTargetWorldPos = getWorldRotation() *
		mModelSettings.msIkTargetPos + glm::vec3(worldPos.x, 0.0f, worldPos.y);


}