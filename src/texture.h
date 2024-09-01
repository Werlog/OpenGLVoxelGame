#pragma once

#include <string>

class Texture {
public:
	void loadTexture(const std::string& path);
	unsigned int getTextureHandle();
	int getWidth() const;
	int getHeight() const;
private:
	unsigned int textureHandle;
	int width, height;
};