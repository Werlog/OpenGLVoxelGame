#include "Game.h"

#include <glad/glad.h>
#include <iostream>
#include <openglErrorReporting.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "texture.h"
#include "texturesheet.h"
#include "chunk.h"
#include "world.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imguiThemes.h"

Game::Game() : player(true, (float)windowWidth / (float)windowHeight)
{

}

bool Game::init()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(windowWidth, windowHeight, "I am almost Minecraft", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create a GLFW window" << std::endl;
		return false;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return false;
	}

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glViewport(0, 0, windowWidth, windowHeight);

	ImGui::CreateContext();
	//ImGui::StyleColorsDark();
	//imguiThemes::yellow();
	//imguiThemes::gray();
	//imguiThemes::green();
	//imguiThemes::red();
	imguiThemes::embraceTheDarkness();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	io.FontGlobalScale = 2.0f;

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		//style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 0.f;
		style.Colors[ImGuiCol_DockingEmptyBg].w = 0.f;
	}

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(glDebugOutput, 0);
	
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	/*
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(2.0f);
	*/

	glEnable(GL_DEPTH_TEST);
}

void Game::gameLoop()
{
	Shader shader;
	shader.loadShader("shaders\\defaultVertex.vert", "shaders\\defaultFragment.frag");

	Texture texture;
	texture.loadTexture("textures\\spritesheet.png");
	TextureSheet sheet = TextureSheet(&texture, 16, 16);

	BlockPalette pallete;
	pallete.registerBlocks();

	glUseProgram(shader.getProgramHandle());

	World world = World(&pallete, &sheet, player, shader.getProgramHandle());
	world.createWorld();
	timer.tick();
	
	unsigned int viewLoc = glGetUniformLocation(shader.getProgramHandle(), "view");
	unsigned int projectionLoc = glGetUniformLocation(shader.getProgramHandle(), "projection");

	glClearColor(0.25f, 0.8f, 1.0f, 1.0f);

	glBindTexture(GL_TEXTURE_2D, texture.getTextureHandle());

	while (!glfwWindowShouldClose(window)) 
	{
		processInput();
		timer.tick();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		player.update(timer.deltaTime, window, world);

		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(player.camera.getViewMatrix()));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(player.camera.getProjectionMatrix()));

		world.update(player, timer.deltaTime);
		world.renderWorld(player);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

bool lastPressed = false;

void Game::processInput()
{
	int esc = glfwGetKey(window, GLFW_KEY_ESCAPE);
	if (esc == GLFW_PRESS && !lastPressed)
	{
		int inputMode = glfwGetInputMode(window, GLFW_CURSOR);
		if (inputMode == GLFW_CURSOR_DISABLED)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		lastPressed = true;
	}
	else if (esc != GLFW_PRESS)
		lastPressed = false;
}

void Game::frameBufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	player.camera.updateProjectionMatrix((float)width/(float)height);
}

void Game::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	player.camera.ProccessMouse(xPos, yPos, true);
}

void Game::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
{
	player.scrollBlockSelection(yOffset);
}