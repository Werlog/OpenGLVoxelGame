#include "camera.h"
#include "world.h"
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include "imgui.h"

constexpr float playerGravity = -19.62f;
constexpr float playerSpeed = 6.0f;
constexpr float playerWidth = 0.6f;
constexpr float playerHeight = 1.9f;

class Player
{
public:
	Camera camera;
	glm::vec3 position;
	bool colliding;

	Player(bool usePhysics, float cameraAspectRatio);

	void update(float deltaTime, GLFWwindow* window, World& world);
private:
	bool usePhysics;
	bool isGrounded;
	glm::vec3 velocity;
	glm::vec3 lastPos;

	void checkGround(World& world);
	void resolveCollisions(World& world);
	glm::vec3 calculateInputDirection(GLFWwindow* window);
};