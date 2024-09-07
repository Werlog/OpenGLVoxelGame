#include <glm/glm.hpp>
#include <vector>
#include "../texturesheet.h"

constexpr float uiElementVerts[] =
{
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
};

constexpr int uiElementIndices[] =
{
	0, 1, 2,
	1, 3, 2,
};

constexpr float uiElementUVs[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};

class UIElement
{
public:
	UIElement(const glm::vec2& position, const glm::vec2& size);

	void setTextureSheet(TextureSheet& sheet, int textureIndex);
	void setTexture(Texture* texture);
	
	void updateMesh();
	void render();
private:
	std::vector<float> uvData;
	Texture* texture;

	unsigned int VAO;
	unsigned int VBO;
	unsigned int EBO;

	glm::vec2 position;
	glm::vec2 size;
};