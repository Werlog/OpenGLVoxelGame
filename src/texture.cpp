#include "texture.h"
#include <glad/glad.h>
#include <stb_image/stb_image.h>
#include <iostream>

void Texture::loadTexture(const std::string& path)
{
	stbi_set_flip_vertically_on_load(1);
	glGenTextures(1, &textureHandle);
	glBindTexture(GL_TEXTURE_2D, textureHandle);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int channels;
	unsigned char* data = stbi_load(std::string(RESOURCES_PATH + path).c_str(), &width, &height, &channels, 0);

	if (data)
	{
		if (channels != 3 && channels != 4)
		{
			std::cout << "Failed to load the texture at " << path << ": unsupported number of channels " << channels << std::endl;
			stbi_image_free(data);
			return;
		}
		GLint format = channels == 3 ? GL_RGB : GL_RGBA;

		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture at " << path << std::endl;
	}

	stbi_image_free(data);
}

int Texture::getWidth() const
{
	return width;
}

int Texture::getHeight() const
{
	return height;
}

unsigned int Texture::getTextureHandle()
{
	return textureHandle;
}