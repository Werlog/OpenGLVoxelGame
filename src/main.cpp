#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <openglErrorReporting.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

#include "Game.h"

// Couldn't get callbacks for the 'game' object to work otherwise so I did this stupid thing
Game game;

void frame_buffer_size_callback(GLFWwindow* window, int width, int height)
{
	game.frameBufferSizeCallback(window, width, height);
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
	game.mouseCallback(window, xPos, yPos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	game.scrollCallback(window, xoffset, yoffset);
}

int main(void)
{
	if (!game.init())
	{
		glfwTerminate();
		return 1;
	}

	glfwSetFramebufferSizeCallback(game.window, frame_buffer_size_callback);
	glfwSetCursorPosCallback(game.window, mouse_callback);
	glfwSetScrollCallback(game.window, scroll_callback);

	game.gameLoop();

	return 0;
}

