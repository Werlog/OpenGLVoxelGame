#include "world.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <cmath>

World::World(BlockPalette* pallete, TextureSheet* sheet, int shaderHandle)
{
	this->pallete = pallete;
	this->sheet = sheet;
	this->shaderProgram = shaderHandle;

	shaderModelLoc = glGetUniformLocation(shaderProgram, "model");
}

World::~World()
{
	for (size_t i = 0; i < chunks.size(); i++)
	{
		delete chunks[i];
	}
}

void World::createWorld()
{
	chunks.reserve(WORLD_SIZE_X * WORLD_SIZE_Z);

	srand(time(NULL));

	siv::PerlinNoise::seed_type seed = rand() % 100000;

	perlinNoise = siv::PerlinNoise(seed);

	for (int x = 0; x < WORLD_SIZE_X; x++)
	{
		for (int z = 0;  z < WORLD_SIZE_Z;  z++)
		{
			ChunkCoord coord = ChunkCoord{ x, z };
			Chunk* chunk = new Chunk(pallete, this, coord, &perlinNoise);
			chunk->generateChunk();
			chunk->updateMesh(*sheet);

			chunks.push_back(chunk);
		}
	}
}

void World::renderWorld()
{
	for (size_t i = 0; i < chunks.size(); i++)
	{
		Chunk* chunk = chunks[i];
		ChunkCoord coord = chunk->position;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(coord.x * CHUNK_SIZE_X, 0, coord.y * CHUNK_SIZE_Z + 1));
		glUniformMatrix4fv(shaderModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		chunk->renderChunk();
	}
}

Chunk* World::getChunkByCoordinate(ChunkCoord coord)
{
	for (size_t i = 0; i < chunks.size(); i++)
	{
		if (chunks[i]->position == coord)
		{
			return chunks[i];
		}
	}

	return nullptr;
}

unsigned char World::getBlockAt(int x, int y, int z)
{
	ChunkCoord coord = ChunkCoord::toChunkCoord(glm::vec3(x, y, z));
	Chunk* chunk = getChunkByCoordinate(coord);
	if (chunk == nullptr) return 0;

	return chunk->getBlockAt(x - coord.x * CHUNK_SIZE_X, y, z - coord.y * CHUNK_SIZE_Z);
}

unsigned char World::getBlockAt(float x, float y, float z)
{
	int xCheck = (int)floor(x);
	int yCheck = (int)floor(y);
	int zCheck = (int)floor(z);

	return getBlockAt(xCheck, yCheck, zCheck);
}