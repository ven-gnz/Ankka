#pragma once
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "tiny_gltf.h"

class Texture
{
public:
	bool loadTexture(std::string filename, bool flipImage);
	bool loadTextureFromBinary(tinygltf::Image& img);
	void bind();
	void unbind();
	void cleanup();

private:
	GLuint mTexture = 0;
	int mTexWidth = 0;
	int mTexHeight = 0;
	int mNumberOfChannels = 0;
	std::string mTextureName;
};