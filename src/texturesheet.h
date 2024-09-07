#pragma once

#include "texture.h"
#include <vector>
#include <glm/glm.hpp>

class TextureSheet 
{
public:
	TextureSheet(Texture* sheet, int textureWidth, int textureHeight);

	std::vector<float> getUVs(int textureNum);
	float getOneUnitX() const;
	float getOneUnitY() const;
	Texture* getTexture() const;
private:
	Texture* sheet;
	int textureWidth, textureHeight;
	float oneUnitX, oneUnitY;
};