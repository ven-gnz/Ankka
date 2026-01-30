#include "Ankka/GltfModel.h"
#include "Ankka/Logger.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
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

void GltfModel::uploadIndexBuffer()
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);
	const tinygltf::BufferView& indexBufferView = mModel->bufferViews[indexAccessor.bufferView];
	const tinygltf::Buffer& indexBuffer = mModel->buffers[indexBufferView.buffer];

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexVBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indexBufferView.byteLength, &indexBuffer.data.at(0) +
		indexBufferView.byteOffset, GL_STATIC_DRAW);
}

int GltfModel::getTriangleCount()
{
	const tinygltf::Primitive& primitives = mModel->meshes.at(0).primitives.at(0);

	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);
	return indexAccessor.count;
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
	const tinygltf::Accessor& indexAccessor = mModel->accessors.at(primitives.indices);

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
	switch (indexAccessor.componentType)
	{
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: indexType = GL_UNSIGNED_BYTE; break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: indexType = GL_UNSIGNED_SHORT; break;
	case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT: indexType = GL_UNSIGNED_INT; break;
	default: Logger::log(1, "error unknown index component type");
	}
	mTex.bind();
	glBindVertexArray(mVAO);

	glDrawElements(drawMode, indexAccessor.count, indexType, nullptr);

	glBindVertexArray(0);
	mTex.unbind();

}

glm::mat4& GltfModel::modelMatrix()
{
	return mModelMatrix;
}

