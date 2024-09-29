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
#include <random>
#include <limits>

World::World(BlockPalette* pallete, TextureSheet* sheet, Player& player, int shaderHandle)
{
	this->pallete = pallete;
	this->sheet = sheet;
	this->shaderProgram = shaderHandle;
	this->isGenerating = false;
	this->isUpdatingChunk = false;

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
	loadedChunks.reserve(RENDER_DISTANCE * RENDER_DISTANCE);

	std::random_device rd;
	std::mt19937 generator(rd());

	std::uniform_int_distribution<> distr(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());

	int seed = distr(generator);
	splinedGenerator.setSeed(seed);

	std::cout << "World Seed is: " << seed << std::endl;

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

	if (!chunksToUpdate.empty())
	{
		Chunk* chunk = chunksToUpdate.front();
		if (isUpdatingChunk && chunk->readyToUpdate.load())
		{
			chunk->createMesh();
			chunksToUpdate.pop_front();
			isUpdatingChunk = false;
		}
		else if (!isUpdatingChunk)
		{
			chunk->updateMesh(*sheet);
			isUpdatingChunk = true;
		}
	}

	if (!chunksToLoad.empty() && sinceLoadedChunk > loadChunkDelay)
	{
		Chunk* chunk = chunksToLoad.front();
		if (getChunkByCoordinate(chunk->position) != nullptr)
		{
			chunksToLoad.pop_front();
			isGenerating = false;
			return;
		}
		if (!isGenerating)
		{
			chunk->generateChunk();
			isGenerating = true;
			return;
		}
		if (chunk->generated.load())
		{
			std::lock_guard lock(chunksMutex);
			chunksToLoad.pop_front();
			chunksToUpdate.push_back(chunk);

			applyBlockMods(true);

			if (isFirstTimeLoading && chunksToLoad.empty())
			{
				isFirstTimeLoading = false;
			}

			loadedChunks.push_back(chunk);
			sinceLoadedChunk = 0.0f;
			isGenerating = false;
		}

	}
	else
	{
		sinceLoadedChunk += deltaTime;
	}
}

void World::updateLoadedChunks(ChunkCoord& playerCoord)
{
	std::lock_guard lock(chunksMutex);
	for (int x = -RENDER_DISTANCE; x < RENDER_DISTANCE; x++)
	{
		for (int z = -RENDER_DISTANCE; z < RENDER_DISTANCE; z++) 
		{
			ChunkCoord coord = ChunkCoord{ playerCoord.x + x, playerCoord.y + z };
			Chunk* unloaded = getUnloadedChunkByCoordinate(coord);
			if (getChunkByCoordinate(coord) == nullptr && unloaded == nullptr)
			{
				Chunk* chunk = new Chunk(pallete, this, coord, &splinedGenerator);
				chunksToLoad.push_back(chunk);
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
			if (std::find(chunksToUnload.begin(), chunksToUnload.end(), chunk) != chunksToUnload.end()) continue;

			chunksToUnload.push_back(chunk);
		}
	}

	unloadChunks();
}

void World::unloadChunks()
{
	std::lock_guard lock(chunksMutex);

	for (int i = 0; i < chunksToUnload.size(); i++)
	{
 		Chunk* chunk = chunksToUnload[i];
		if (!chunk->isUpdating.load() && chunk->generated.load())
		{
			if (!chunksToLoad.empty() && chunk == chunksToLoad.front()) continue;
			if (!chunksToUpdate.empty() && chunk == chunksToUpdate.front()) continue;

			loadedChunks.erase(std::remove(loadedChunks.begin(), loadedChunks.end(), chunk), loadedChunks.end());
			chunksToLoad.erase(std::remove(chunksToLoad.begin(), chunksToLoad.end(), chunk), chunksToLoad.end());
			chunksToUpdate.erase(std::remove(chunksToUpdate.begin(), chunksToUpdate.end(), chunk), chunksToUpdate.end());

			chunksToUnload.erase(chunksToUnload.begin() + i);
			if (chunk->modified)
			{
				unloadedChunks.push_back(chunk);
			}
			else
			{
				delete chunk;
			}
			i--;
		}
	}

	std::cout << chunksToUnload.size() << std::endl;
}

void World::renderWorld(Player& player)
{
	std::lock_guard lock(chunksMutex);
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
			this->chunksToUpdate.push_back(chunk);
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
	chunk->setBlockAt(chunkX, y, chunkZ, newBlockType);
	if (chunkX + 1 >= CHUNK_SIZE_X)
	{
		Chunk* xChunk = getChunkByCoordinate(ChunkCoord{coord.x + 1, coord.y});
		if (xChunk != nullptr)
		{
			chunksToUpdate.push_back(xChunk);
		}
	}
	else if (chunkX - 1 <= 0)
	{
		Chunk* xChunk = getChunkByCoordinate(ChunkCoord{ coord.x - 1, coord.y });
		if (xChunk != nullptr)
		{
			chunksToUpdate.push_back(xChunk);
		}
	}
	if (chunkZ + 1 >= CHUNK_SIZE_Z)
	{
		Chunk* zChunk = getChunkByCoordinate(ChunkCoord{coord.x, coord.y + 1});
		if (zChunk != nullptr)
		{
			chunksToUpdate.push_back(zChunk);
		}
	}
	else if (chunkZ - 1 <= 0)
	{
		Chunk* zChunk = getChunkByCoordinate(ChunkCoord{ coord.x, coord.y - 1 });
		if (zChunk != nullptr)
		{
			chunksToUpdate.push_back(zChunk);
		}
	}

	chunksToUpdate.push_back(chunk);
}

Chunk* World::getChunkByCoordinate(ChunkCoord coord)
{
	for (size_t i = 0; i < loadedChunks.size(); i++)
	{
		if (coord == loadedChunks[i]->position)
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
		if (includeNotGenerated) return Chunk::getGenerateBlockAt(splinedGenerator, x, y, z);
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