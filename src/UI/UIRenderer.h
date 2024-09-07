#include "UIElement.h"
#include "../Shader.h"
#include <vector>

class UIRenderer
{
public:
	UIRenderer();

	void addUIElement(UIElement* element);

	void render();
private:
	Shader uiShader;
	std::vector<UIElement*> uiElements;
};