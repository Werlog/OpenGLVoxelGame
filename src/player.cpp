#include "player.h"
#include <iostream>
#include <string>
#include <algorithm>

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
		velocity.y = 1050.0f * deltaTime;
	}

	movementDirection += velocity;


	position += movementDirection * deltaTime;
	resolveCollisions(world);

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
	for (float x = -0.3f; x <= 0.3f; x += 0.3f)
	{
		for (float z = -0.3f; z <= 0.3f; z += 0.3f)
		{
			isGrounded = world.getBlockAt(position.x + x, position.y - 0.01f, position.z + z);
			if (isGrounded) return;
		}
	}
}

void Player::resolveCollisions(World& world)
{
	/*
		This is kind of janky right now and
		I will have to improve it later on
		but it will do for now.
	*/
	bool foundCollision = true;
	int i = 0;
	while (foundCollision && i < 50)
	{
		unsigned char collidingBlock = 0;
		glm::vec3 blockPos;
		for (float x = -(playerWidth * 0.5f); x <= playerWidth * 0.5f; x += playerWidth)
		{
			for (float y = 0; y <= playerHeight; y += playerHeight)
			{
				for (float z = -(playerWidth * 0.5f); z <= playerWidth * 0.5f; z += playerWidth)
				{
					unsigned char block = world.getBlockAt(position.x + x, position.y + y, position.z + z);
					if (block != 0)
					{
						blockPos.x = floor(position.x + x);
						blockPos.y = floor(position.y + y);
						blockPos.z = floor(position.z + z);
						collidingBlock = block;
						break;
					}
				}
			}
		}

		if (collidingBlock != 0)
		{
			float overlapX = std::min(position.x + playerWidth * 0.5f, blockPos.x + 1.0f) - std::max(position.x - playerWidth * 0.5f, blockPos.x);
			float overlapY = std::min(position.y + playerHeight, blockPos.y + 1.0f) - std::max(position.y, blockPos.y);
			float overlapZ = std::min(position.z + playerWidth * 0.5f, blockPos.z + 1.0f) - std::max(position.z - playerWidth * 0.5f, blockPos.z);
			overlapX += 0.001f;
			overlapZ += 0.001f;


			if (overlapY < overlapX && overlapY < overlapZ)
			{
				if (position.y < blockPos.y)
				{
					position.y -= overlapY;
				}
				else
				{
					position.y += overlapY;
				}
			}
			else if (overlapX < overlapY && overlapX < overlapZ)
			{
				if (position.x < blockPos.x + 0.5f)
				{
					position.x -= overlapX;
				}
				else
				{
					position.x += overlapX;
				}
			}
			else if (overlapZ < overlapY && overlapZ < overlapX)
			{
				if (position.z < blockPos.z + 0.5f)
				{
					position.z -= overlapZ;
				}
				else
				{
					position.z += overlapZ;
				}
			}
		}
		else foundCollision = false;
		i++;
	}
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