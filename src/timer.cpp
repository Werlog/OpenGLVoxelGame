#include "timer.h"
#include <GLFW/glfw3.h>

Timer::Timer()
{
	lastTime = glfwGetTime();
	deltaTime = 0.0f;
}

void Timer::tick()
{
	deltaTime = glfwGetTime() - lastTime;

	lastTime = glfwGetTime();
}