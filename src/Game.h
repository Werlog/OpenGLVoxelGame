#include <GLFW/glfw3.h>
#include "player.h"
#include "timer.h"

constexpr int windowWidth = 800;
constexpr int windowHeight = 600;

class Game 
{
public:
	GLFWwindow* window;

	Game();

	bool init();
	void gameLoop();

	void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
	void mouseCallback(GLFWwindow* window, double xPos, double yPos);
private:
	float lastMouseX, lastMouseY;
	Player player;
	Timer timer;

	void processInput();
};