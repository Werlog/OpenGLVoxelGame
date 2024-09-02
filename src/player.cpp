#include "player.h"
#include <iostream>
#include <string>

Player::Player(bool usePhysics, float cameraAspectRatio) : camera(glm::vec3(0, 60, 0), 60, cameraAspectRatio, 10.0f)
{
	this->usePhysics = usePhysics;
	this->position = glm::vec3(0, 80, 0);
	colliding = false;
	isGrounded = false;
	this->lastPos = position;
	this->velocity = glm::vec3(0);
}

void Player::update(float deltaTime, GLFWwindow* window, World& world)
{
	glm::vec3 inputDirection = calculateInputDirection(window);

	checkGround(world);

	glm::vec3 forward = glm::vec3(camera.front.x, camera.front.y, camera.front.z);
	glm::vec3 right = glm::vec3(camera.right.x, camera.right.y, camera.right.z);

	glm::vec3 movementDirection = forward * inputDirection.z + right * inputDirection.x;
	movementDirection.y = 0;
	if (movementDirection != glm::vec3(0))
		movementDirection = glm::normalize(movementDirection);

	movementDirection *= playerSpeed;

	if (usePhysics && !isGrounded) 
	{
		velocity.y += playerGravity * deltaTime;
	}
	else if (isGrounded)
	{
		velocity.y = 0.0f;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && isGrounded)
	{
		velocity.y = 800.0f * deltaTime;
	}

	movementDirection += velocity;

	position += movementDirection * deltaTime;
	camera.position = position + glm::vec3(0.0f, 1.8f, 0.0f);

	ImGui::Begin("Debug info");
	float dist = glm::distance(lastPos, position);
	ImGui::Text("Distance: %f", dist);
	ImGui::Text("Position: %f %f %f", position.x, position.y, position.z);
	ChunkCoord coord = ChunkCoord::toChunkCoord(position);
	ImGui::Text("Chunk Coord: %d %d", coord.x, coord.y);
	ImGui::End();

	lastPos = position;
}

void Player::checkGround(World& world)
{
	for (float x = -0.25f; x < 0.25f; x += 0.25f)
	{
		for (float z = -0.25f; z < 0.25f; z += 0.25f)
		{
			isGrounded = world.getBlockAt(position.x + x, position.y - 0.01f, position.z + z);
			if (isGrounded) return;
		}
	}
}

void Player::resolveCollisions(World& world)
{

}

glm::vec3 Player::calculateInputDirection(GLFWwindow* window)
{
	glm::vec3 inputDir = glm::vec3(0);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		inputDir.z += 1;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		inputDir.z -= 1;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		inputDir.x -= 1;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		inputDir.x += 1;
	}

	return inputDir;
}