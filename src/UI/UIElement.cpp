#include "UIElement.h"
#include <glad/glad.h>

UIElement::UIElement(const glm::vec2& position, const glm::vec2& size)
{
	this->position = position;
	this->size = size;

	this->uvData.reserve(4);

	VAO = 0;
	VBO = 0;
	EBO = 0;
}

void UIElement::setTextureSheet(TextureSheet& sheet, int textureIndex)
{
	texture = sheet.getTexture();
	uvData = sheet.getUVs(textureIndex);
}

void UIElement::setTexture(Texture* texture)
{
	this->texture = texture;
	uvData.clear();
	for (int i = 0; i < sizeof(uiElementUVs) / sizeof(float); i++)
	{
		uvData.push_back(uiElementUVs[i]);
	}
}

void UIElement::updateMesh()
{
	if (uvData.empty()) return;

	if (VAO == 0)
		glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	if (VBO == 0)
		glGenBuffers(1, &VBO);
	if (EBO == 0)
		glGenBuffers(1, &EBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);


	std::vector<float> vertexData;
	int uvCounter = 0;
	for (size_t i = 0; i < sizeof(uiElementVerts) / sizeof(float); i += 3)
	{
		vertexData.push_back(uiElementVerts[i]);
		vertexData.push_back(uiElementVerts[i + 1]);
		vertexData.push_back(uiElementVerts[i + 2]);
		vertexData.push_back(uvData[uvCounter]);
		vertexData.push_back(uvData[uvCounter + 1]);

		uvCounter += 2;
	}

	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uiElementIndices), uiElementIndices, GL_DYNAMIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void UIElement::render()
{
	if (uvData.empty()) return;

	glBindTexture(GL_TEXTURE_2D, texture->getTextureHandle());
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}