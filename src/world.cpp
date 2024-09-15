#include "world.h"
#include "glad/glad.h"
#include "glm/glm.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <algorithm>
#include <cmath>
#include "profiling/codetimer.h"
#include "player.h"

World::World(BlockPalette* pallete, TextureSheet* sheet, Player& player, int shaderHandle)
{
	this->pallete = pallete;
	this->sheet = sheet;
	this->shaderProgram = shaderHandle;

	lastPlayerChunkCoord = ChunkCoord::toChunkCoord(player.position);
	sinceLoadedChunk = 0.0f;

	isFirstTimeLoading = true;

	shaderModelLoc = glGetUniformLocation(shaderProgram, "model");
}

World::~World()
{
	for (size_t i = 0; i < loadedChunks.size(); i++)
	{
		delete loadedChunks[i];
	}
}

void World::createWorld()
{
	CodeTimer createTimer = CodeTimer("World creation");
	loadedChunks.reserve(RENDER_DISTANCE * RENDER_DISTANCE);

	srand(time(NULL));

	perlinNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	perlinNoise.SetSeed(rand() % RAND_MAX);

	updateLoadedChunks(lastPlayerChunkCoord);
}

void World::update(Player& player, float deltaTime)
{
	if (isFirstTimeLoading)
	{
		player.position.y = 80.0f;
		player.velocity.y = 0.0f;
	}
	ChunkCoord playerCoord = ChunkCoord::toChunkCoord(player.position);
	if (playerCoord != lastPlayerChunkCoord)
	{
		updateLoadedChunks(playerCoord);

		lastPlayerChunkCoord = playerCoord;
	}

	if (!chunksToLoad.empty() && sinceLoadedChunk > loadChunkDelay)
	{
		Chunk* chunk = chunksToLoad.front();
		chunksToLoad.pop();
		if (getChunkByCoordinate(chunk->position) != nullptr) return;

		chunk->generateChunk();
		chunk->updateMesh(*sheet);

		applyBlockMods(true);

		if (isFirstTimeLoading && chunksToLoad.empty())
		{
			isFirstTimeLoading = false;
		}

		loadedChunks.push_back(chunk);
		sinceLoadedChunk = 0.0f;
	}
	else
	{
		sinceLoadedChunk += deltaTime;
	}
}

void World::updateLoadedChunks(ChunkCoord& playerCoord)
{
	for (int x = -RENDER_DISTANCE; x < RENDER_DISTANCE; x++)
	{
		for (int z = -RENDER_DISTANCE; z < RENDER_DISTANCE; z++) 
		{
			ChunkCoord coord = ChunkCoord{ playerCoord.x + x, playerCoord.y + z };
			Chunk* unloaded = getUnloadedChunkByCoordinate(coord);
			if (getChunkByCoordinate(coord) == nullptr && unloaded == nullptr)
			{
				Chunk* chunk = new Chunk(pallete, this, coord, &perlinNoise);
				chunksToLoad.push(chunk);
			}
			else if (unloaded != nullptr)
			{
				unloadedChunks.erase(std::remove(unloadedChunks.begin(), unloadedChunks.end(), unloaded), unloadedChunks.end());
				loadedChunks.push_back(unloaded);
			}
		}
	}

	for (int i = 0; i < loadedChunks.size(); i++)
	{
		Chunk* chunk = loadedChunks[i];
		if (abs(chunk->position.x - playerCoord.x) > RENDER_DISTANCE
			|| abs(chunk->position.y - playerCoord.y) > RENDER_DISTANCE)
		{
			if (chunk->modified)
			{
				unloadedChunks.push_back(chunk);
			}
			else
			{
				delete chunk;
			}
			loadedChunks.erase(loadedChunks.begin() + i);
			i--;
		}
	}
}

void World::renderWorld(Player& player)
{
	for (size_t i = 0; i < loadedChunks.size(); i++)
	{
		Chunk* chunk = loadedChunks[i];
		ChunkCoord coord = chunk->position;

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(coord.x * CHUNK_SIZE_X, 0, coord.y * CHUNK_SIZE_Z));
		glUniformMatrix4fv(shaderModelLoc, 1, GL_FALSE, glm::value_ptr(model));

		chunk->renderChunk();
	}
}

void World::addBlockMods(std::vector<BlockMod>& mods)
{
	for (BlockMod mod : mods)
	{
		blocksToGenerate.push_back(mod);
	}
}

void World::applyBlockMods(bool updateChunks = true)
{
	std::vector<Chunk*> chunksToUpdate;

	for (int i = 0; i < blocksToGenerate.size(); i++)
	{
		BlockMod& blockMod = blocksToGenerate[i];
		ChunkCoord coord = ChunkCoord::toChunkCoord(blockMod.blockX, blockMod.blockZ);
		Chunk* chunk = getChunkByCoordinate(coord);
		if (chunk == nullptr) continue;

		int chunkX = blockMod.blockX - coord.x * CHUNK_SIZE_X;
		int chunkZ = blockMod.blockZ - coord.y * CHUNK_SIZE_Z;
		chunk->setBlockAtDontUpdate(chunkX, blockMod.blockY, chunkZ, blockMod.blockType);

		if (updateChunks && std::find(chunksToUpdate.begin(), chunksToUpdate.end(), chunk) == chunksToUpdate.end())
		{
			chunksToUpdate.push_back(chunk);
		}

		blocksToGenerate.erase(blocksToGenerate.begin() + i);
		i--;
	}
	if (updateChunks)
	{
		for (Chunk* chunk : chunksToUpdate)
		{
			chunk->updateMesh(*sheet);
		}
	}
}

void World::modifyBlockAt(int x, int y, int z, unsigned char newBlockType)
{
	ChunkCoord coord = ChunkCoord::toChunkCoord(x, z);
	Chunk* chunk = getChunkByCoordinate(coord);
	if (chunk == nullptr) return;

	chunk->modified = true;
	int chunkX = x - coord.x * CHUNK_SIZE_X;
	int chunkZ = z - coord.y * CHUNK_SIZE_Z;
	chunk->setBlockAt(chunkX, y, chunkZ, newBlockType, *sheet);
	if (chunkX + 1 >= CHUNK_SIZE_X)
	{
		Chunk* xChunk = getChunkByCoordinate(ChunkCoord{coord.x + 1, coord.y});
		if (xChunk != nullptr)
		{
			xChunk->updateMesh(*sheet);
		}
	}
	else if (chunkX - 1 <= 0)
	{
		Chunk* xChunk = getChunkByCoordinate(ChunkCoord{ coord.x - 1, coord.y });
		if (xChunk != nullptr)
		{
			xChunk->updateMesh(*sheet);
		}
	}
	if (chunkZ + 1 >= CHUNK_SIZE_Z)
	{
		Chunk* zChunk = getChunkByCoordinate(ChunkCoord{coord.x, coord.y + 1});
		if (zChunk != nullptr)
		{
			zChunk->updateMesh(*sheet);
		}
	}
	else if (chunkZ - 1 <= 0)
	{
		Chunk* zChunk = getChunkByCoordinate(ChunkCoord{ coord.x, coord.y - 1 });
		if (zChunk != nullptr)
		{
			zChunk->updateMesh(*sheet);
		}
	}
}

Chunk* World::getChunkByCoordinate(ChunkCoord coord)
{
	for (size_t i = 0; i < loadedChunks.size(); i++)
	{
		if (loadedChunks[i]->position == coord)
		{
			return loadedChunks[i];
		}
	}

	return nullptr;
}

Chunk* World::getUnloadedChunkByCoordinate(ChunkCoord coord)
{
	for (size_t i = 0; i < unloadedChunks.size(); i++)
	{
		if (unloadedChunks[i]->position == coord)
		{
			return unloadedChunks[i];
		}
	}

	return nullptr;
}

unsigned char World::getBlockAt(int x, int y, int z, bool includeNotGenerated = false)
{
	if (y < 0 || y > CHUNK_SIZE_Y) return 0;

	ChunkCoord coord = ChunkCoord::toChunkCoord(glm::vec3(x, y, z));
	Chunk* chunk = getChunkByCoordinate(coord);
	if (chunk == nullptr)
	{
		if (includeNotGenerated) return Chunk::getGenerateBlockAt(perlinNoise, x, y, z);
		return 0;
	}
	return chunk->getBlockAt(x - coord.x * CHUNK_SIZE_X, y, z - coord.y * CHUNK_SIZE_Z);
}

unsigned char World::getBlockAt(float x, float y, float z)
{
	int xCheck = (int)floor(x);
	int yCheck = (int)floor(y);
	int zCheck = (int)floor(z);

	return getBlockAt(xCheck, yCheck, zCheck);
}