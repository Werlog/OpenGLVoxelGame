#pragma once

#include "chunk.h"
#include <queue>
#include "splinedgenerator.h"
#include <mutex>

constexpr int RENDER_DISTANCE = 8;
constexpr float loadChunkDelay = 0.0f;

class Player;

class World
{
public:
	std::recursive_mutex chunksMutex;

	World(BlockPalette* pallete, TextureSheet* sheet, Player& player, int shaderHandle);
	~World();

	void createWorld();
	void update(Player& player, float deltaTime);
	void updateLoadedChunks(ChunkCoord& playerCoord);
	void unloadChunks();
	void renderWorld(Player& player);
	void addBlockMods(std::vector<BlockMod>& mods);
	void applyBlockMods(bool updateChunks);
	void modifyBlockAt(int x, int y, int z, unsigned char newBlockType);
	unsigned char getBlockAt(int x, int y, int z, bool includeNotGenerated);
	unsigned char getBlockAt(float x, float y, float z);
	Chunk* getChunkByCoordinate(ChunkCoord coord);
	Chunk* getUnloadedChunkByCoordinate(ChunkCoord coord);
private:
	std::vector<Chunk*> loadedChunks;
	std::vector<Chunk*> unloadedChunks;
	std::deque<Chunk*> chunksToLoad;
	std::vector<Chunk*> chunksToUnload;
	std::deque<Chunk*> chunksToUpdate;

	SplinedGenerator splinedGenerator;

	float sinceLoadedChunk;
	bool isFirstTimeLoading;
	bool isGenerating;
	bool isUpdatingChunk;

	int shaderProgram;
	int shaderModelLoc;

	std::vector<BlockMod> blocksToGenerate;
	TextureSheet* sheet;
	BlockPalette* pallete;
	ChunkCoord lastPlayerChunkCoord;
};