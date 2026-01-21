
#include "opengl/Texture.h"
#include "Ankka/Logger.h"
#include <../external/tinygltf/stb_image.h>

bool Texture::loadTexture(std::string textureFilename)
{
	int mTexWidth, mTexHeight, mNumberOfChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* textureData = stbi_load(textureFilename.c_str(), &mTexWidth, &mTexHeight, &mNumberOfChannels, 0);

	if (!textureData)
	{
		stbi_image_free(textureData);
		Logger::log(1, "s%: No texture data found!", __FUNCTION__);
		return false;
	}

	glGenTextures(1, &mTexture);
	glBindTexture(GL_TEXTURE_2D, mTexture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, mTexWidth, mTexHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(textureData);
	return true;
}

void Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, mTexture);
}

void Texture::unbind()
{
	glBindTexture(GL_TEXTURE_2D, 0);
}