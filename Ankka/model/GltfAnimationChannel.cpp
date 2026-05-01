#include <model/GltfAnimationChannel.h>


void GltfAnimationChannel::loadChannelData(
	std::shared_ptr<tinygltf::Model> model,
	tinygltf::Animation anim,
	tinygltf::AnimationChannel channel
)
{
	mTargetNode = channel.target_node;

	const tinygltf::Accessor& inputAccessor = model->accessors.at(anim.samplers.at(channel.sampler).input);
	const tinygltf::BufferView& inputBufferView = model->bufferViews.at(inputAccessor.bufferView);
	const tinygltf::Buffer& inputBuffer = model->buffers.at(inputBufferView.buffer);

	std::vector<float> timings;
	timings.resize(inputAccessor.count);

	std::memcpy(timings.data(),
		&inputBuffer.data.at(0) + inputBufferView.byteOffset,
		inputBufferView.byteLength);

	setTimings(timings);

	const tinygltf::AnimationSampler sampler = anim.samplers.at(channel.sampler);

	if (sampler.interpolation.compare("STEP") == 0)
	{
		mInterType = EInterpolationType::STEP;
	}
	else if (sampler.interpolation.compare("LINEAR") == 0)
	{
		mInterType = EInterpolationType::LINEAR;
	}
	else
	{
		mInterType = EInterpolationType::CUBICSPLINE;
	}

	const tinygltf::Accessor& outputAccessor = model->accessors.at(anim.samplers.at(channel.sampler).output);
	const tinygltf::BufferView& outputBufferView = model->bufferViews.at(outputAccessor.bufferView);
	const tinygltf::Buffer& outputBuffer = model->buffers.at(outputBufferView.buffer);

	//TODO check for right member variables to update :

	if (channel.target_path.compare("rotation") == 0)
	{
		mTargetPath = ETargetPath::ROTATION;
		std::vector<glm::quat> rotations;
		rotations.resize(outputAccessor.count);
		
		std::memcpy(rotations.data(),
			&outputBuffer.data.at(0) + outputBufferView.byteOffset,
			outputBufferView.byteLength);
		setRotations(rotations);

	}
	else if (channel.target_path.compare("translation") == 0)
	{
		mTargetPath = ETargetPath::TRANSLATION;
		std::vector<glm::vec3> translations;
		translations.resize(outputAccessor.count);
		std::memcpy(translations.data(),
			&outputBuffer.data.at(0) + outputBufferView.byteLength,
			outputBufferView.byteLength);
		setTranslations(translations);

	}
	else
	{
		mTargetPath = ETargetPath::SCALE;
		std::vector<glm::vec3> scale;
		scale.resize(outputAccessor.count);
		std::memcpy(scale.data(),
			&outputBuffer.data.at(0) + outputBufferView.byteOffset,
			outputBufferView.byteLength);
		setScalings(scale);
	}
}