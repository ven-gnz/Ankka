#include <model/ModelLoader.h>

bool ModelLoader::loadGltfModel
(
	const std::string& filename,
	std::shared_ptr<tinygltf::Model>&model,
	std::string& errors,
	std::string& warnings
)
{
	tinygltf::TinyGLTF loader;

	model = std::make_shared<tinygltf::Model>();

	bool result = false;

	if (filename.ends_with(".glb"))
	{
		result = loader.LoadBinaryFromFile(
			model.get(),
			&errors,
			&warnings,
			filename);
	}
	else
	{
		result = loader.LoadASCIIFromFile(
			model.get(),
			&errors,
			&warnings,
			filename);
	}
	return result;
}