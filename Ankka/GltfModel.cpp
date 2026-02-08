#include "Ankka/GltfModel.h"
#include "Ankka/Logger.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>

void GltfModel::createIndexBuffer()
{
	glGenBuffers(1, &mIndexVBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);

}

void GltfModel::createVertexBuffers()
{

	const tinygltf::Primitive& primitives =
		mModel->meshes.at(0).primitives.at(0); // since model only contains one mesh, this is fine for now
	mVertexVBO.resize(primitives.attributes.size());

	for (const auto& attrib : primitives.attributes)
	{
		const std::string attribType = attrib.first;
		const int accessorNum = attrib.second;

		const tinygltf::Accessor& accessor = mModel->accessors.at(accessorNum);
		const tinygltf::BufferView& bufferView = mModel->bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = mModel->buffers[bufferView.buffer];

		if ((attribType.compare("POSITION") != 0) &&
			(attribType.compare("NORMAL") != 0) &&
			attribType.compare("TEXCOORD_0") != 0) {
			continue;
			}
		// Loop for AABB max and min
		if (attribType == "POSITION")
		{
			const unsigned char* dataP = buffer.data.data() + bufferView.byteOffset + accessor.byteOffset;

			glm::vec3 meshMin(FLT_MAX);
			glm::vec3 meshMax(-FLT_MAX);

			size_t stride = accessor.ByteStride(bufferView);
			if (stride == 0) stride = sizeof(float) * 3;

			for (size_t i = 0; i < accessor.count; ++i)
			{
				const float* pos = reinterpret_cast<const float*>(dataP + stride * i);
				glm::vec3 vert(pos[0], pos[1], pos[2]);

				meshMin = glm::min(meshMin, vert);
				meshMax = glm::max(meshMax, vert);
			}

			mLocalAABBmin = meshMin;
			mLocalAABBmax = meshMax;

		}

		int dataSize = 1;
		switch (accessor.type)
		{
		case TINYGLTF_TYPE_SCALAR:
			dataSize = 1;
			break;
		case TINYGLTF_TYPE_VEC2:
			dataSize = 2;
			break;
		case TINYGLTF_TYPE_VEC3:
			dataSize = 3;
			break;
		case TINYGLTF_TYPE_VEC4:
			dataSize = 4;
			break;
		default:
			Logger::log(1, "$s error : accessor %i uses data size %i\n", __FUNCTION__, accessorNum, dataSize);
			break;
		}

		GLuint dataType = GL_FLOAT;
		switch (accessor.componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_FLOAT:
			dataType = GL_FLOAT;
			break;
		default:
			Logger::log(1, "%s error: accessor %i uses uknown data type %i\n", __FUNCTION__, accessorNum, dataType);
			break;
		}



		glGenBuffers(1, &mVertexVBO[attributes[attribType]]);
		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[attributes[attribType]]);

		glBufferData(GL_ARRAY_BUFFER,
			bufferView.byteLength,
			buffer.data.data() + bufferView.byteOffset + accessor.byteOffset,
			GL_STATIC_DRAW);

		size_t stride = accessor.ByteStride(bufferView);
		glVertexAttribPointer(attributes[attribType], dataSize, dataType, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(attributes[attribType]);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	std::string minStr = glm::to_string(mLocalAABBmin);
	std::string maxStr = glm::to_string(mLocalAABBmax);

	Logger::log(1, "%s: AABB smallest: %s\n", __FUNCTION__, minStr.c_str());
	Logger::log(1, "%s: AABB biggest  : %s\n", __FUNCTION__, maxStr.c_str());

}

void GltfModel::uploadVertexBuffers()
{
	const tinygltf::Primitive& primitives = mModel->meshes[0].primitives[0];

	for (const auto& attrib : primitives.attributes)
	{
		const std::string& attribType = attrib.first;
		int accessorNum = attrib.second;

		if (attributes.find(attribType) == attributes.end()) continue;

		const tinygltf::Accessor& accessor = mModel->accessors.at(accessorNum);
		const tinygltf::BufferView& bufferView = mModel->bufferViews[accessor.bufferView];
		const tinygltf::Buffer& buffer = mModel->buffers[bufferView.buffer];

		glBindBuffer(GL_ARRAY_BUFFER, mVertexVBO[attributes[attribType]]);
		glBufferData(GL_ARRAY_BUFFER, bufferView.byteLength, buffer.data.data() + bufferView.byteOffset + accessor.byteOffset, GL_STATIC_DRAW);
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
			indexBufferView.byteLength, &indexBuffer.data.at(0) +
			indexBufferView.byteOffset, GL_STATIC_DRAW);
	}
}

void GltfModel::getJointData()
{
	std::string jointsAccessorAttrib = "JOINTS_0";
	int jointsAccessor = mModel->meshes.at(0).primitives.at(0).attributes.at(jointsAccessorAttrib);
	Logger::log(1, "%s : using accessor %i to get %s\n", __FUNCTION__, jointsAccessor,
		jointsAccessorAttrib.c_str());

	const tinygltf::Accessor &accessor = mModel->accessors.at(jointsAccessor);
	const tinygltf::BufferView &bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer &buffer = mModel->buffers.at(bufferView.buffer);

	int jointVecSize = accessor.count;
	Logger::log(1, "%s: %i short vec4 in JOINTS_0\n", __FUNCTION__, jointVecSize);
	mJointVec.resize(jointVecSize);

	std::memcpy(
		mJointVec.data(), 
		&buffer.data.at(0) + bufferView.byteOffset, 
		bufferView.byteLength);

	mNodeToJoint.resize(mModel->nodes.size());

	const tinygltf::Skin& skin = mModel->skins.at(0);
	for (int i = 0; i < skin.joints.size(); ++i)
	{
		int destinationNode = skin.joints.at(i);
		mNodeToJoint.at(destinationNode) = i;
		Logger::log(1, "%s: joint %i affects node %i\n", __FUNCTION__, i, destinationNode);
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

	std::memcpy(
		mWeightVec.data(), 
		&buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength);

}

void GltfModel::getInvBindMatrices()
{
	const tinygltf::Skin& skin = mModel->skins.at(0);
	int invBindMatAccessor = skin.inverseBindMatrices;

	const tinygltf::Accessor& accessor = mModel->accessors.at(invBindMatAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	mInverseBindMatrices.resize(skin.joints.size());
	mJointMatrices.resize(skin.joints.size());

	std::memcpy(
		mInverseBindMatrices.data(),
		&buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength
	);

}

std::shared_ptr<OGLMesh> GltfModel::getSkeleton(bool enableSkinning)
{
	mSkeletonMesh->vertices.resize(mModel->nodes.size() * 2);
	mSkeletonMesh->vertices.clear();

	getSkeletonPerNode(mRootNode->getChilds().at(0), enableSkinning);
	return mSkeletonMesh;
}

void GltfModel::getSkeletonPerNode(std::shared_ptr<GltfNode> treeNode, bool enableSkinning)
{
	glm::vec3 parentPos = glm::vec3(0.0f);

	if (enableSkinning)
	{
		parentPos = glm::vec3(treeNode->getNodeMatrix()[3]);
	}
	else
	{
		glm::mat4 bindMatrix = glm::inverse(mInverseBindMatrices.at(mNodeToJoint.at(treeNode->getNodeNum())));
		parentPos = bindMatrix * treeNode->getNodeMatrix()[3];
	}
	OGLVertex parentVertex;
	parentVertex.position = parentPos;
	parentVertex.color = glm::vec3(0.0f, 1.0f, 1.0f);

	for (const auto& childNode : treeNode->getChilds())
	{
		// transform child
		glm::vec3 childPos = glm::vec3(0.0f);
		if (enableSkinning)
		{
			childPos = glm::vec3(childNode->getNodeMatrix()[3]);
		}
		else
		{
			glm::mat4 bindMatrix = glm::inverse(mInverseBindMatrices.at(mNodeToJoint.at(childNode->getNodeNum())));
			childPos = bindMatrix * childNode->getNodeMatrix()[3];
		}
		// update state
		OGLVertex childVertex;
		childVertex.position = childPos;
		childVertex.color = glm::vec3(0.0f, 0.0f, 1.0f);
		mSkeletonMesh->vertices.emplace_back(parentVertex);
		mSkeletonMesh->vertices.emplace_back(childVertex);
		// recurse down
		getSkeletonPerNode(childNode, enableSkinning);
	}

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

void GltfModel::getNodeData(std::shared_ptr<GltfNode> treeNode, glm::mat4 parentNodeMatrix)
{
	int nodeNum = treeNode->getNodeNum();
	const tinygltf::Node& node = mModel->nodes.at(nodeNum);
	treeNode->setNodeName(node.name);

	if (node.translation.size())
	{
		treeNode->setTranslation(glm::make_vec3(node.translation.data()));
	}
	if (node.rotation.size())
	{
		treeNode->setRotation(glm::make_quat(node.rotation.data()));
	}
	if (node.scale.size())
	{
		treeNode->setScale(glm::make_vec3(node.scale.data()));
	}
	treeNode->calculateLocalTRSMatrix();
	treeNode->calculateNodeMatrix(parentNodeMatrix);

	mJointMatrices.at(mNodeToJoint.at(nodeNum)) =
		treeNode->getNodeMatrix() * mInverseBindMatrices.at(mNodeToJoint.at(nodeNum));
	
}

void GltfModel::getNodes(std::shared_ptr<GltfNode> treeNode)
{
	int nodeNum = treeNode->getNodeNum();
	std::vector<int> childNodes = mModel->nodes.at(nodeNum).children;

	auto removeIt = std::remove_if(childNodes.begin(), childNodes.end(),
		[&](int num) { return mModel->nodes.at(num).skin != -1; });

	childNodes.erase(removeIt, childNodes.end());

	treeNode->addChilds(childNodes);
	glm::mat4 treeNodeMatrix = treeNode->getNodeMatrix();

	for (auto& childNode : treeNode->getChilds())
	{
		getNodeData(childNode, treeNodeMatrix);
		getNodes(childNode);
	}

}

void GltfModel::getInvBindMatrices()
{
	const tinygltf::Skin& skin = mModel->skins.at(0);
	int invBindMatAccessor = skin.inverseBindMatrices;

	const tinygltf::Accessor& accessor = mModel->accessors.at(invBindMatAccessor);
	const tinygltf::BufferView& bufferView = mModel->bufferViews.at(accessor.bufferView);
	const tinygltf::Buffer& buffer = mModel->buffers.at(bufferView.buffer);

	mInverseBindMatrices.resize(skin.joints.size());
	mJointMatrices.resize(skin.joints.size());

	std::memcpy(mInverseBindMatrices.data(), &buffer.data.at(0) + bufferView.byteOffset,
		bufferView.byteLength);
}

bool GltfModel::loadModel(OGLRenderData& renderData,
	std::string modelFileName,
	std::string textureFileName)
{

	std::string loaderErrors;
	std::string loaderWarnings;
	tinygltf::TinyGLTF gltfLoader;
	mModel = std::make_shared<tinygltf::Model>();
	bool result = false;

	if (modelFileName.ends_with(".glb"))
	{

		std::ifstream f(modelFileName, std::ios::binary | std::ios::ate);
		if (!f)
		{
			Logger::log(1, "Failed to open: %s\n", modelFileName.c_str());
			return false;
		}

		result = gltfLoader.LoadBinaryFromFile(mModel.get(),
			&loaderErrors,
			&loaderWarnings,
			modelFileName,
			0);
		if (!result)
		{
			Logger::log(1, "Failed to load: %s\n", modelFileName.c_str());
			return false;
		}
	
		const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
		if (primitives.indices < 0)
		{
			const tinygltf::Accessor& posAccessor =
				mModel->accessors[primitives.attributes.at("POSITION")];

			mVertexCount = posAccessor.count;
		}

			tinygltf::Image& img = mModel->images[0];

			if (!mTex.loadTextureFromBinary(img)) 
			{
				return false;
			}



	}

	else
	{
		result = gltfLoader.LoadASCIIFromFile(mModel.get(), &loaderErrors, &loaderWarnings, modelFileName);

		if (!mTex.loadTexture(textureFileName, false), false)
		{
			return false;
		}
	}

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
		Logger::log(1, "s error : could not load file '%s'\n", __FUNCTION__, modelFileName.c_str());
		return false;
	}

	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);
	createVertexBuffers();
	createIndexBuffer();
	glBindVertexArray(0);

	getJointData();
	getWeightData();
	getInvBindMatrices();

	int nodeCount = mModel->nodes.size();
	int rootNode = mModel->scenes.at(0).nodes.at(0);
	mRootNode = GltfNode::createRoot(rootNode);
	getNodeData(mRootNode, glm::mat4(1.0f));
	getNodes(mRootNode);
	

	mSkeletonMesh = std::make_shared<OGLMesh>();

	mRootNode->printTree();
	renderData.rdTriangelCount = getTriangleCount();
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

void GltfModel::draw()
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);

	GLuint drawMode = GL_TRIANGLES;
	switch (primitives.mode)
	{
	case TINYGLTF_MODE_TRIANGLES:
		drawMode = GL_TRIANGLES;
		break;
	default:
		Logger::log(1, "s error: unknowd draw mode %i\n", __FUNCTION__, drawMode);
		break;
	}
	GLenum indexType = 0;
	const tinygltf::Accessor* indexAccessor = nullptr;
	
	if (primitives.indices >= 0)
	{
		indexAccessor = &mModel->accessors.at(primitives.indices);
		switch (indexAccessor->componentType)
		{
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: indexType = GL_UNSIGNED_BYTE; break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: indexType = GL_UNSIGNED_SHORT; break;
		case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: indexType = GL_UNSIGNED_INT; break;
		default: Logger::log(1, "error unknown index component type");
			
		}

	}
	mTex.bind();
	glBindVertexArray(mVAO);
	if (indexAccessor)
	{
		glDrawElements(drawMode, indexAccessor->count, indexType, nullptr);
	}
	else
	{
		glDrawArrays(drawMode, 0, mVertexCount);
	}
	
	glBindVertexArray(0);
	mTex.unbind();

}

glm::mat4& GltfModel::modelMatrix()
{
	return mModelMatrix;
}

