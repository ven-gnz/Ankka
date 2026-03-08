#pragma once

#include <memory>
#include <string>
#include <tiny_gltf.h>

class ModelLoader
{
public:
	static bool loadGltfModel(
		const std::string& filename,
		std::shared_ptr<tinygltf::Model>& model,
		std::string& errors,
		std::string& warnings);
};