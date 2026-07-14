#include "model/GltfModel.h"
#include "tools/Logger.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/dual_quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <fstream>
#include <iostream>
#include <chrono>
#include <cmath>

void GltfModel::createIndexBuffer()
{
	glGenBuffers(1, &mIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);

}


GLenum GltfModel::getGLComponentType(const tinygltf::Accessor& accessor, int accessorNum)
{
	GLuint dataType = GL_FLOAT;
	switch (accessor.componentType) {
	case TINYGLTF_COMPONENT_TYPE_FLOAT:
		return GL_FLOAT;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
		return GL_UNSIGNED_SHORT;
	default:
		Logger::log(1, "%s error: accessor %i uses unknown data type %i\n", __FUNCTION__,
			accessorNum, accessor.componentType);
		break;
	}
	return dataType;
}

int GltfModel::getComponentCount(const tinygltf::Accessor& accessor, int accessorNum)
{

	switch (accessor.type) {
	case TINYGLTF_TYPE_SCALAR:
		return 1;
	case TINYGLTF_TYPE_VEC2:
		return 2;
	case TINYGLTF_TYPE_VEC3:
		return 3;
	case TINYGLTF_TYPE_VEC4:
		return 4;
	default:
		Logger::log(1, "%s error: accessor %i uses data size %i\n", __FUNCTION__,
			accessorNum, accessor.type);
		return -1;
	}
	
}

void GltfModel::createPrimitive(const tinygltf::Primitive& tinyPrimitive, GltfPrimitive& primitive)
{
	glGenVertexArrays(1, &primitive.vao);
	glBindVertexArray(primitive.vao);

	for (const auto& attrib : tinyPrimitive.attributes)
	{
		const std::string& attribType = attrib.first;
		const tinygltf::Accessor& accessor = mModel->accessors.at(attrib.second);
		Logger::log(1, " %-12s type=%d count=%zu component=%d\n",
				attrib.first.c_str(),
				accessor.type,
				accessor.count,
				accessor.componentType);
	}

	if (tinyPrimitive.material >= 0)
	{
		const tinygltf::Material& material = mModel->materials.at(tinyPrimitive.material);
		int textureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
		if (textureIndex >= 0)
		{
			const tinygltf::Texture& texture = mModel->textures.at(textureIndex);
			const tinygltf::Image& image = mModel->images.at(texture.source);
			primitive.tex = Texture::loadTextureFromBinary(image);
			Logger::log(1, "Texture id = %u\n", primitive.tex);

			GLint w = 0;

			glBindTexture(GL_TEXTURE_2D, primitive.tex);

			glGetTexLevelParameteriv(
				GL_TEXTURE_2D,
				0,
				GL_TEXTURE_WIDTH,
				&w);

			Logger::log(1, "Uploaded width = %d", w);
		}
	}
	if (mTex.mTexture >= 0)
	{
		primitive.tex = mTex.mTexture;
		Logger::log(1, "\nCopying texture id :%zu\n", mTex.mTexture);

		Logger::log(1,
			"createPrimitive: GltfModel %p  mTex=%p  texture=%u",
			this,
			&mTex,
			mTex.mTexture);
	}

	for (const auto& attrib : tinyPrimitive.attributes)
	{

		const std::string& attribType = attrib.first;
		int accessorNum = attrib.second;

		const tinygltf::Accessor& accessor = mModel->accessors.at(accessorNum);
		const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

		if ((attribType.compare("POSITION") != 0) && (attribType.compare("NORMAL") != 0)
			&& (attribType.compare("TEXCOORD_0") != 0) && (attribType.compare("JOINTS_0") != 0
				&& (attribType.compare("WEIGHTS_0") != 0))) {
			Logger::log(1, "%s: skipping attribute type %s\n", __FUNCTION__, attribType.c_str());
			continue;
		}
		//CPU copies for CPU skin
		const uint8_t* data =
			buffer.data.data()
			+ bufferView.byteOffset
			+ accessor.byteOffset;
		if (attribType == "POSITION")
		{
			primitive.vertexCount = accessor.count;
			const glm::vec3* src =
				reinterpret_cast<const glm::vec3*>(data);
			primitive.positions.assign(src, src + accessor.count);
		}
		if (attribType == "NORMAL")
		{
			primitive.vertexCount = accessor.count;
			const glm::vec3* src =
				reinterpret_cast<const glm::vec3*>(data);
			primitive.normals.assign(src, src + accessor.count);
		}
		if (attribType == "JOINTS_0"){
			const glm::u16vec4* src = reinterpret_cast<const glm::u16vec4*>(data);
			primitive.joints.assign(src, src + accessor.count);
		}
		if (attribType == "WEIGHTS_0")
		{
			const glm::vec4* src = reinterpret_cast<const glm::vec4*>(data);
			primitive.weights.assign(src, src + accessor.count);
		}

		int location = attributes.at(attribType);
		primitive.accessors[location] = accessorNum;

		glGenBuffers(1, &primitive.vbos[location]);
		glBindBuffer(GL_ARRAY_BUFFER, primitive.vbos[location]);

		glVertexAttribPointer(
			location,
			getComponentCount(accessor, accessorNum),
			getGLComponentType(accessor, accessorNum),
			accessor.normalized ? GL_TRUE : GL_FALSE,
			bufferView.byteStride,
			(void*)0
		);

		Logger::log(1,
			"%s stride=%d",
			attribType.c_str(),
			bufferView.byteStride);

		glEnableVertexAttribArray(location);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void GltfModel::uploadPrimitiveBuffers(GltfPrimitive& primitive)
{
	glBindVertexArray(primitive.vao);
	for (size_t i = 0; i < primitive.vbos.size(); ++i)
	{
		if (primitive.vbos[i] == 0) continue;

		const tinygltf::Accessor& accessor =
			mModel->accessors.at(primitive.accessors[i]);

		const tinygltf::BufferView& bufferView =
			mModel->bufferViews.at(accessor.bufferView);

		const tinygltf::Buffer& buffer =
			mModel->buffers.at(bufferView.buffer);

		glBindBuffer(GL_ARRAY_BUFFER, primitive.vbos[i]);

		// the accessors bytestride itself is handled by the vertex attrib pointer
		glBufferData(
			GL_ARRAY_BUFFER,
			accessor.count *
			getComponentCount(accessor, primitive.accessors[i]) *
			tinygltf::GetComponentSizeInBytes(accessor.componentType),
			buffer.data.data() +
			bufferView.byteOffset +
			accessor.byteOffset,
			GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void GltfModel::createIndexBuffer(
	const tinygltf::Primitive& tinyPrimitive,
	GltfPrimitive& primitive)
{
	if (tinyPrimitive.indices < 0)
	{
		primitive.indexCount = 0;
		return;
	}

	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(tinyPrimitive.indices);
	const tinygltf::BufferView& indexBufferView = mModel->bufferViews[indexAccessor.bufferView];
	const tinygltf::Buffer& indexBuffer = mModel->buffers[indexBufferView.buffer];
	primitive.indexType = indexAccessor.componentType; // use of getGLComponent helper necessary?
	primitive.indexCount = indexAccessor.count;

	glBindVertexArray(primitive.vao);
	glGenBuffers(1, &primitive.ebo);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, primitive.ebo);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		indexAccessor.count * tinygltf::GetComponentSizeInBytes(indexAccessor.componentType),
		indexBuffer.data.data() + indexBufferView.byteOffset + indexAccessor.byteOffset,
		GL_STATIC_DRAW);
	glBindVertexArray(0);
}

void GltfModel::createMeshes()
{
	for (const tinygltf::Mesh tinyMesh : mModel->meshes)
	{

		GltfMesh mesh;
		for (const tinygltf::Primitive& tinyPrimitive : tinyMesh.primitives)
		{
			GltfPrimitive primitive;
			Logger::log(1, "Primitive attributes : \n");
			createPrimitive(tinyPrimitive, primitive);
			uploadPrimitiveBuffers(primitive);
			createIndexBuffer(tinyPrimitive, primitive);

			mesh.primitives.push_back(std::move(primitive));
		}
		mMeshes.push_back(std::move(mesh));
	}
}


void GltfModel::createVertexBuffers()
{

	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);

	mVertexVBO.resize(primitives.attributes.size());
	mAttribAccessors.resize(primitives.attributes.size());

	for (const auto& attrib : primitives.attributes)
	{
		const std::string attribType = attrib.first;
		const int accessorNum = attrib.second;

		const tinygltf::Accessor &accessor = mModel->accessors.at(accessorNum);
		const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

		if ((attribType.compare("POSITION") != 0) && (attribType.compare("NORMAL") != 0)
			&& (attribType.compare("TEXCOORD_0") != 0) && (attribType.compare("JOINTS_0") != 0
				&& (attribType.compare("WEIGHTS_0") != 0))) {
			Logger::log(1, "%s: skipping attribute type %s\n", __FUNCTION__, attribType.c_str());
			continue;
		}

		Logger::log(1, "%s: data for %s uses accessor %i\n", __FUNCTION__, attribType.c_str(),
			accessorNum);
		if (attribType.compare("POSITION") == 0) {
			int numPositionEntries = accessor.count;
			Logger::log(1, "%s: loaded %i vertices from glTF file\n", __FUNCTION__,
				numPositionEntries);
		}

		mAttribAccessors.at(attributes.at(attribType)) = accessorNum;

		int dataSize = getComponentCount(accessor, accessorNum);
		GLenum dataType = getGLComponentType(accessor, accessorNum);
		
		/* buffers for position, normal and tex coordinates */
		glGenBuffers(1, &mVertexVBO.at(attributes.at(attribType)));
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO.at(attributes.at(attribType)));

		glVertexAttribPointer(attributes.at(attribType), dataSize, dataType, GL_FALSE,
			bufferView.byteStride, (void*)0);
		glEnableVertexAttribArray(attributes.at(attribType));

		glBindBuffer(GL_ARRAY_BUFFER, 0);

	}
}

void GltfModel::uploadVertexBuffers()
{

	
	for (size_t i = 0; i < 5; ++i)
	{
		const tinygltf::Accessor& accessor = mModel->accessors.at(mAttribAccessors.at(i));
		const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
		const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO.at(i));

		// changed the start of the data pointer with the intent that the accessor offsets is handled when constructing the vertex buffer
		glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength,
			&buffer.data.at(0) + bufferView.byteOffset, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
}


// If the model does not contain indices, ie the data is laid out in a triangle friendly order -> no index buffer needed
void GltfModel::uploadIndexBuffer()
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
	if (primitives.indices >= 0)
	{
		const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);
		const tinygltf::BufferView& indexBufferView = mModel->bufferViews[indexAccessor.bufferView];
		const tinygltf::Buffer& indexBuffer = mModel->buffers[indexBufferView.buffer];

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
			indexBufferView.byteLength, 
			&indexBuffer.data.at(0) + indexBufferView.byteOffset,
			GL_STATIC_DRAW);
	}
}

void GltfModel::getJointData()
{
	
	std::string jointsAccessorAttrib = "JOINTS_0";
	int jointsAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(jointsAccessorAttrib);
	Logger::log(1, "%s: using accessor %i to get %s\n", __FUNCTION__, jointsAccessor,
		jointsAccessorAttrib.c_str());

	const tinygltf::Accessor& accessor = mModel->accessors.at(jointsAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	int jointVecSize = accessor.count;
	Logger::log(1, "%s: %i short vec4 in JOINTS_0\n", __FUNCTION__, jointVecSize);
	mJointVec.resize(jointVecSize);

	std::memcpy(mJointVec.data(), &buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength);

	mNodeToJoint.resize(mModel->nodes.size());

	const tinygltf::Skin& skin = mModel->skins.at(0);
	for (int i = 0; i < skin.joints.size(); ++i) {
		int destinationNode = skin.joints.at(i);
		mNodeToJoint.at(destinationNode) = i;
		Logger::log(2, "%s: joint %i affects node %i\n", __FUNCTION__, i, destinationNode);
	}
}

void GltfModel::getWeightData()
{
	std::string weightsAccessorAttrib = "WEIGHTS_0";
	int weightAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(weightsAccessorAttrib);
	Logger::log(1, "%s: using accessor %i to get %s\n", __FUNCTION__, weightAccessor,
		weightsAccessorAttrib.c_str());

	const tinygltf::Accessor &accessor = mModel->accessors.at(weightAccessor);
	const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

	int weightVecSize = accessor.count;
	Logger::log(1, "%s: %i vec4 in WEIGHTS_0\n", __FUNCTION__, weightVecSize);
	mWeightVec.resize(weightVecSize);

	const unsigned char* start = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;
	int elemCount = accessor.count * sizeof(glm::vec4);

	std::memcpy(
		mWeightVec.data(), 
		start,
		elemCount);

}

std::vector<glm::mat4> GltfModel::getInverseBindMatrices()
{

	return mInverseBindMatrices;

}

void GltfModel::getInvBindMatrices()
{
	const tinygltf::Skin& skin = mModel->skins.at(0);
	int invBindMatAccessor = skin.inverseBindMatrices;

	const tinygltf::Accessor& accessor = mModel->accessors.at(invBindMatAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	mInverseBindMatrices.resize(skin.joints.size());

	std::memcpy(mInverseBindMatrices.data(), &buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength);
}





int GltfModel::getTriangleCount()
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
	// indexed geometry
	if (primitives.indices >= 0)
	{
		const tinygltf::Accessor& indexAccessor =
			mModel->accessors[primitives.indices];
		return indexAccessor.count;
	}
	// non indexed geo
	else
	{
		const tinygltf::Accessor& posAccessor =
			mModel->accessors[primitives.attributes.at("POSITION")];
		return posAccessor.count;
	}

}

void GltfModel::getNodeData(std::shared_ptr<GltfNode> treeNode)
{
	//Logger::log(1,
	//	"getNodeData: treeNode->getNodeNum() = %d\n",
	//	treeNode->getNodeNum());
	int nodeNum = treeNode->getNodeNum();
	const tinygltf::Node& node = mModel->nodes.at(nodeNum);
	//Logger::log(1,
	//	"tiny node '%s' mesh=%d skin=%d\n",
	//	node.name.c_str(),
	//	node.mesh,
	//	node.skin);
	treeNode->setNodeName(node.name);

	if (node.translation.size()) {
		treeNode->setTranslation(glm::make_vec3(node.translation.data()));
	}
	else {
		treeNode->setTranslation(glm::vec3(0.0f));
	}

	if (node.rotation.size()) {
		treeNode->setRotation(glm::make_quat(node.rotation.data()));
	}
	else {
		treeNode->setRotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f));
	}

	if (node.scale.size()) {
		treeNode->setScale(glm::make_vec3(node.scale.data()));
	}
	else {
		treeNode->setScale(glm::vec3(1.0f));
	}
	if (node.mesh >= 0)
	{
		treeNode->setMeshIndex(node.mesh);
		if (node.skin >= 0) {
			treeNode->setSkinIndex(node.skin);
			//Logger::log(1,
			//	"Node %d '%s' mesh=%d skin=%d children=%zu",
			//	nodeNum,
			//	node.name.c_str(),
			//	node.mesh,
			//	node.skin,
			//	node.children.size());

			//Logger::log(1,
			//	"Stored mesh index = %d",
			//	treeNode->getMeshIndex());
		}
		else {
			//Logger::log(
			//	1,
			//	"Node %d '%s': mesh=%d children=%zu",
			//	nodeNum,
			//	node.name.c_str(),
			//	node.mesh,
			//	node.children.size());
		}
	}

	treeNode->calculateNodeMatrix();

}

// 14.07. retired the removal of the skinned nodes to favor the less-hardcoded path.
// In the book the mesh and skin were loaded as separate efforts(as they should be), 
// but in the new intermediate model the idea is to shoot for a simple but correct architecture that does not hardcode the tree to end on the skinned mesh
// Further yet the mesh on the model was actually placed as the child so the descent not leading to leaves left the mesh index of the node to not update to point to the mesh.
// This is of course a temporary solution until I can figure out a way to unify the paths.

void GltfModel::getNodes(std::shared_ptr<GltfNode> treeNode)
{
	int nodeNum = treeNode->getNodeNum();
	std::vector<int> childNodes = mModel->nodes.at(nodeNum).children;

	// This was the hardcoded approach 
	//auto removeIt = std::remove_if(childNodes.begin(), childNodes.end(),
	//	[&](int num) { return mModel->nodes.at(num).skin != -1; });

	//childNodes.erase(removeIt, childNodes.end());

	treeNode->addChilds(childNodes);
	glm::mat4 treeNodeMatrix = treeNode->getNodeMatrix();

	for (auto& childNode : treeNode->getChilds())
	{
		getNodeData(childNode);
		getNodes(childNode);
	}

}


// debug method
void GltfModel::calculateBindPose()
{
	for (auto& skin : mSkins)
	{
		skin.jointMatrices.resize(skin.joints.size());

		for (size_t i = 0; i < skin.joints.size(); ++i) {
			int nodeIndex = skin.joints[i];
			skin.jointMatrices[i] = 
				mDebugNodeList[nodeIndex]->getNodeMatrix() * skin.inverseBindMatrices[i];

		}
		for (auto& mesh : mMeshes) {
			for (auto& primitive : mesh.primitives) {
				for (size_t v = 0; v < primitive.positions.size(); ++v)
				{
					const glm::u16vec4& joints = primitive.joints[v];
					const glm::vec4& weights = primitive.weights[v];

					glm::mat4 skinMatrix =
						weights.x * skin.jointMatrices[joints.x]
						+ weights.y * skin.jointMatrices[joints.y]
						+ weights.z * skin.jointMatrices[joints.z]
						+ weights.w * skin.jointMatrices[joints.w];

					glm::vec4 p = glm::vec4(primitive.positions[v], 1.0f);

					primitive.positions[v] = glm::vec3(skinMatrix * p);
				}
			}
		}
	}

}


void GltfModel::loadSkins()
{
	mSkins.resize(mModel->skins.size());

	for (size_t skinIndex = 0; skinIndex < mModel->skins.size(); ++skinIndex)
	{
		const tinygltf::Skin& source = mModel->skins[skinIndex];
		GltfSkin& destination = mSkins[skinIndex];

		destination.name = source.name;
		destination.joints = source.joints;

		if (source.inverseBindMatrices >= 0)
		{
			const tinygltf::Accessor& accessor = mModel->accessors[source.inverseBindMatrices];
			const tinygltf::BufferView& view = mModel->bufferViews[accessor.bufferView];
			const tinygltf::Buffer& buffer = mModel->buffers[view.buffer];

			destination.inverseBindMatrices.resize(accessor.count);
			std::memcpy(
				destination.inverseBindMatrices.data(),
				buffer.data.data() +
				view.byteOffset +
				accessor.byteOffset,
				accessor.count * sizeof(glm::mat4));
		}
		destination.jointMatrices.resize(destination.joints.size());
	}

}



bool GltfModel::loadModel(OGLRenderData& renderData,
	std::string modelFilename,
	std::string textureFilename,
	bool useMeshPrimitiveApproach,
	bool isInstanced)
{

	if (textureFilename.empty() && modelFilename.ends_with(".glb"))
	{
		Logger::log(1, " Loading binary file with textures hopefuly baked into the model \n");
		
	}


	else
	{
		if (!mTex.loadTexture(textureFilename, false)) {
			Logger::log(1, "%s: texture loading failed\n", __FUNCTION__);
			return false;
		}
	}

	mModel = std::make_shared<tinygltf::Model>();

	tinygltf::TinyGLTF gltfLoader;
	std::string loaderErrors;
	std::string loaderWarnings;
	bool result = false;

	if (modelFilename.ends_with(".gltf"))
	{
		result = gltfLoader.LoadASCIIFromFile(mModel.get(), &loaderErrors, &loaderWarnings,
			modelFilename);

		if (!loaderWarnings.empty())
		{
			Logger::log(1, "%s: warnings while loading glTF model :\n%s\n", __FUNCTION__, loaderWarnings.c_str());
		}

		if (!loaderErrors.empty())
		{
			Logger::log(1, "%s: errors while loading glTF model :\n%s\n", __FUNCTION__, loaderErrors.c_str());
		}

		if (!result)
		{
			Logger::log(1, "%s error : could not load file '%s'\n", __FUNCTION__, modelFilename.c_str());
			return false;
		}
	}
	else if (modelFilename.ends_with(".glb"))
	{
		result = gltfLoader.LoadBinaryFromFile(mModel.get(), &loaderErrors, &loaderWarnings, modelFilename, 0);

		if (!loaderWarnings.empty())
		{
			Logger::log(1, "%s: warnings while loading glTF model :\n%s\n", __FUNCTION__, loaderWarnings.c_str());
		}

		if (!loaderErrors.empty())
		{
			Logger::log(1, "%s: errors while loading glTF model :\n%s\n", __FUNCTION__, loaderErrors.c_str());
		}

		if (!result)
		{
			Logger::log(1, "%s error : could not load file '%s'\n", __FUNCTION__, modelFilename.c_str());
			return false;
		}
	}


	mModelFilename = modelFilename;
	if (!useMeshPrimitiveApproach)
	{
		glGenVertexArrays(1, &mVAO);
		glBindVertexArray(mVAO);
		createVertexBuffers();
		createIndexBuffer();

		glBindVertexArray(0);
	}
	else
	{
		createMeshes();
		loadSkins();
	}

	if (!useMeshPrimitiveApproach)
	{
		getJointData();
		getWeightData();
		getInvBindMatrices();
	}

		
	
	mNodeCount = mModel->nodes.size();
	if (useMeshPrimitiveApproach)
	{
		GltfNodeData nodeData = getGltfNodes();
		mDebugRootNode = nodeData.rootNode;

		mDebugNodeList = std::move(nodeData.nodeList);

		//calculateBindPose();
		//for (auto& mesh : mMeshes)
		//{
		//	for (auto& primitive : mesh.primitives){
		//		glBindVertexArray(primitive.vao);
		//		glBindBuffer(GL_ARRAY_BUFFER, primitive.vbos[0]);
		//		glBufferSubData(
		//			GL_ARRAY_BUFFER,
		//			0,
		//			primitive.positions.size() * sizeof(glm::vec3),
		//			primitive.positions.data());
		//		glBindBuffer(GL_ARRAY_BUFFER, 0);
		//	}
		//}
	}


	
	getAnimations();

	return true;
}

void GltfModel::cleanup()
{
	glDeleteBuffers(mVertexVBO.size(), mVertexVBO.data());
	glDeleteBuffers(1, &mVAO);
	glDeleteBuffers(1, &mIndexVBO);
	mTex.cleanup();
	mModel.reset();
}

void GltfModel::getAnimations() {
	for (const auto& anim : mModel->animations) {
		Logger::log(1, "%s: loading animation '%s' with %i channels\n", __FUNCTION__, anim.name.c_str(), anim.channels.size());
		std::shared_ptr<GltfAnimationClip> clip = std::make_shared<GltfAnimationClip>(anim.name);
		for (const auto& channel : anim.channels) {
			clip->addChannel(mModel, anim, channel);
		}
		mAnimClips.push_back(clip);
	}
}

void GltfModel::drawNodeApproach(Shader& s, bool log)
{
	drawNode(mDebugRootNode, s, log);
}


void GltfModel::drawNode(std::shared_ptr<GltfNode> node, Shader& s, bool log)
{

	s.setM4_Uniform("model", node->getNodeMatrix());
	if (log){
		Logger::log(1,
			"drawNode %d hasMesh=%d meshIndex=%d",
			node->getNodeNum(),
			node->hasMesh(),
			node->getMeshIndex());
		glm::mat4 M = node->getNodeMatrix();
		Logger::logm4(1, M);
	}

	if (node->hasMesh())
	{
	
		mMeshes[node->getMeshIndex()].render();
	}
	for (auto& child : node->getChilds())
	{
		drawNode(child, s, log);
	}
}


void GltfModel::draw() {

	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);


	if (primitives.indices >= 0)
	{
		//std::cout << " debug ";
		const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);

		GLuint drawMode = GL_TRIANGLES;
		switch (primitives.mode) {
		case TINYGLTF_MODE_TRIANGLES:
			drawMode = GL_TRIANGLES;
			break;
		default:
			Logger::log(1, "%s error: unknown draw mode %i\n", __FUNCTION__, primitives.mode);
			break;
		}

		
		mTex.bind();
		glBindVertexArray(mVAO);
		glDrawElements(drawMode, indexAccessor.count, indexAccessor.componentType, nullptr);
		glBindVertexArray(0);
		mTex.unbind();
	}
	


}

void GltfModel::drawInstanced(int instanceCount)
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);

	GLuint drawMode = GL_TRIANGLES;
	switch (primitives.mode)
	{
	case TINYGLTF_MODE_TRIANGLES:
		drawMode = GL_TRIANGLES;
		break;
	default:
		Logger::log(1, "s error : unknown draw mode %i\n", __FUNCTION__, primitives.mode);
		break;
	}

	mTex.bind();
	glBindVertexArray(mVAO);
	glDrawElementsInstanced(drawMode, indexAccessor.count, indexAccessor.componentType, nullptr, instanceCount);
	glBindVertexArray(0);
	mTex.unbind();
}


void GltfModel::resetNodeData(
	std::shared_ptr<GltfNode> treeNode
)
{
	getNodeData(treeNode);
	for (auto& childNode : treeNode->getChilds())
	{

		resetNodeData(childNode);
	}
}


std::vector<std::shared_ptr<GltfNode>> GltfModel::getNodeList(
	std::vector<std::shared_ptr<GltfNode>>& nodeList, int nodeNum
)
{
	for (auto& childNode : nodeList.at(nodeNum)->getChilds())
	{
		int childNodeNum = childNode->getNodeNum();
		nodeList.at(childNodeNum) = childNode;
		getNodeList(nodeList, childNodeNum);
	}
	return nodeList;
}

int GltfModel::getNodeCount()
{
	return mNodeCount;
}

std::vector<int> GltfModel::getNodeToJoint() {
	return mNodeToJoint;
}

GltfNodeData GltfModel::getGltfNodes()
{
	GltfNodeData nodeData{};

	int rootNodeNum = mModel->scenes.at(0).nodes.at(0);

	nodeData.rootNode = GltfNode::createRoot(rootNodeNum);

	getNodeData(nodeData.rootNode);
	getNodes(nodeData.rootNode);

	nodeData.nodeList.resize(mNodeCount);
	Logger::log(1,
		"rootNodeNum = %d, mNodeCount = %d, scene nodes = %zu, total nodes = %zu",
		rootNodeNum,
		mNodeCount,
		mModel->scenes.at(0).nodes.size(),
		mModel->nodes.size());
	nodeData.nodeList.at(rootNodeNum) = nodeData.rootNode;
	getNodeList(nodeData.nodeList, rootNodeNum);

	return nodeData;

}

std::vector<std::shared_ptr<GltfAnimationClip>> GltfModel::getAnimClips()
{
	return mAnimClips;
}