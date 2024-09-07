#include "UIRenderer.h"
#include <glad/glad.h>

UIRenderer::UIRenderer()
{
	uiShader = Shader();
	uiShader.loadShader("shaders\\defaultUIVertex.vert", "shaders\\defaultFragment.vert");
}

void UIRenderer::addUIElement(UIElement* element)
{
	uiElements.push_back(element);
}

void UIRenderer::render()
{
	glUseProgram(uiShader.getProgramHandle());
	for (UIElement* el : uiElements)
	{
		el->render();
	}
}