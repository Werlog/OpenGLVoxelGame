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

constexpr int loadDirections[4][2] = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };

World::World(BlockPalette* pallete, TextureSheet* sheet, Player& player, int shaderHandle)
{
	this->pallete = pallete;
	this->sheet = sheet;
	this->shaderProgram = shaderHandle;

	lastPlayerChunkCoord = ChunkCoord::toChunkCoord(player.position);

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

	std::uniform_int_distribution<int32_t> distr(std::numeric_limits<int32_t>::min(), std::numeric_limits<int32_t>::max());

	int32_t seed = distr(generator);
	splinedGenerator.setSeed(seed);

	std::cout << "World Seed is: " << seed << std::endl;

	updateLoadedChunks(lastPlayerChunkCoord);
}

void World::update(Player& player, float deltaTime)
{
	player.position = glm::vec3(player.position.x, 100.0f, player.position.z);
	ChunkCoord playerCoord = ChunkCoord::toChunkCoord(player.position);
	
	if (playerCoord != lastPlayerChunkCoord)
	{
		updateLoadedChunks(playerCoord);
	}
	{
		std::lock_guard lock(chunksMutex);
		for (size_t i = 0; i < loadingChunks.size(); i++)
		{
			Chunk* chunk = loadingChunks[i];
			if (!chunk->isChunkLoaded.load()) continue;

			chunk->createMesh();
			loadedChunks.push_back(chunk);
			loadingChunks.erase(loadingChunks.begin() + i);
			i--;
		}
		while (loadingChunks.size() < MAX_ASYNC_CHUNK_LOAD && !chunksToLoad.empty())
		{
			ChunkCoord coord = chunksToLoad.front();
			chunksToLoad.pop_front();
			Chunk* chunk = new Chunk(pallete, this, coord, &splinedGenerator);
			chunk->loadChunk(*sheet);
			loadingChunks.push_back(chunk);
		}
	}
}

void World::updateLoadedChunks(ChunkCoord& playerCoord)
{
	std::lock_guard lock(chunksMutex);

	std::queue<ChunkCoord> coordsToLoad;
	std::vector<ChunkCoord> visited;

	coordsToLoad.push(playerCoord);
	int total = 0;
	bool isInitial = true;
	while (!coordsToLoad.empty())
	{
		ChunkCoord coord = coordsToLoad.front();
		coordsToLoad.pop();

		if (!isInitial && !chunkExists(coord) && std::find(chunksToLoad.begin(), chunksToLoad.end(), coord) == chunksToLoad.end())
		{
			chunksToLoad.push_back(coord);
			total++;
		}
		else
		{
			isInitial = false;
		}

		for (const auto& direction : loadDirections)
		{
			ChunkCoord newCoord = ChunkCoord{ coord.x + direction[0], coord.y + direction[1] };

			float xDiff = newCoord.x - playerCoord.x;
			float yDiff = newCoord.y - playerCoord.y;
			bool pass = abs(xDiff) <= RENDER_DISTANCE && abs(yDiff) <= RENDER_DISTANCE;
			if (pass && std::find(visited.begin(), visited.end(), newCoord) == visited.end())
			{
				coordsToLoad.push(newCoord);
				visited.push_back(newCoord);
			}
		}
	}
	lastPlayerChunkCoord = playerCoord;
	std::cout << total << std::endl;
}

void World::unloadChunks()
{

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
	/*
	if (updateChunks)
	{
		for (Chunk* chunk : chunksToUpdate)
		{
			this->chunksToUpdate.push_back(chunk);
		}
	}*/
}

void World::modifyBlockAt(int x, int y, int z, unsigned char newBlockType)
{
	ChunkCoord coord = ChunkCoord::toChunkCoord(x, z);
	Chunk* chunk = getChunkByCoordinate(coord);
	if (chunk == nullptr) return;


	int chunkX = x - coord.x * CHUNK_SIZE_X;
	int chunkZ = z - coord.y * CHUNK_SIZE_Z;
	chunk->setBlockAt(chunkX, y, chunkZ, newBlockType);
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

bool World::chunkExists(ChunkCoord coord)
{
	for (size_t i = 0; i < loadedChunks.size(); i++)
	{
		if (coord == loadedChunks[i]->position)
		{
			return true;
		}
	}

	for (size_t i = 0; i < loadingChunks.size(); i++)
	{
		if (coord == loadingChunks[i]->position)
		{
			return true;
		}
	}
	
	return false;
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