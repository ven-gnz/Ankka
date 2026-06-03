#pragma once
#include <vector>
#include <memory>
#include <glm/glm.hpp>
#include "GltfNode.h"
#include "Ankka/Logger.h"


class IKSolver
{
public:
	IKSolver();
	IKSolver(unsigned int iterations);

	void setNodes(std::vector<std::shared_ptr<GltfNode>> nodes);
	std::shared_ptr<GltfNode> getIkChainRootNode();

	void setNumIterations(unsigned int iterations);

	bool solveCCD(glm::vec3 target);
	bool solveFABRIK(glm::vec3 target);

private:
	std::vector<std::shared_ptr<GltfNode>> mNodes{};

	void solveFABRIKforward(glm::vec3 target);
	void solveFABRIKbackward(glm::vec3 base);

	void calculateBoneLengths();
	std::vector<float> mBoneLengths{};

	void adjustFABRIKNodes();
	std::vector<glm::vec3> mFABRIKNodePositions{};

	unsigned int mIterations = 0;
	float mThreshold = 0.00001f;
};