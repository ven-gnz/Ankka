#include "model/GltfAnimationChannel.h"

class GltfAnimationChannel
{

public:
	void loadChannelData(
		std::shared_ptr<tinygltf::Model> model,
		tinygltf::Animation anim,
		tinygltf::AnimationChannel channel);
	

	int getTargetNode();
	ETargetPath getTargetPath();
};
