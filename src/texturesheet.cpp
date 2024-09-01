#include "texturesheet.h"
#include <iterator>
#include <iostream>

TextureSheet::TextureSheet(Texture* sheet, int textureWidth, int textureHeight)
{
	this->sheet = sheet;
	this->textureWidth = textureWidth;
	this->textureHeight = textureHeight;

	oneUnitX = (float)textureWidth / (float)sheet->getWidth();
	oneUnitY = (float)textureHeight / (float)sheet->getHeight();
}

std::vector<float> TextureSheet::getUVs(int textureNum)
{
	int y = (textureNum * textureWidth) / sheet->getWidth();
	int x = textureNum - (y * (sheet->getWidth() / textureWidth));
	float percentageY = 1.0f - ((float)((y * textureHeight)) / (float)sheet->getHeight());
	percentageY -= oneUnitY;
	float percentageX = (float)(x * textureWidth) / (float)sheet->getWidth();

	std::vector<float> uvs = std::vector<float>();
	uvs.reserve(8);
	uvs.push_back(percentageX); uvs.push_back(percentageY);
	uvs.push_back(percentageX + oneUnitX); uvs.push_back(percentageY);
	uvs.push_back(percentageX); uvs.push_back(percentageY + oneUnitY);
	uvs.push_back(percentageX + oneUnitX); uvs.push_back(percentageY + oneUnitY);

	return uvs;
}

float TextureSheet::getOneUnitX() const
{
	return oneUnitX;
}

float TextureSheet::getOneUnitY() const
{
	return oneUnitY;
}