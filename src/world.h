#pragma once

#include "chunk.h"
#include <queue>

constexpr int RENDER_DISTANCE = 7;
constexpr float loadChunkDelay = 0.0f;

class Player;

class World
{
public:
	World(BlockPalette* pallete, TextureSheet* sheet, Player& player, int shaderHandle);
	~World();

	void createWorld();
	void update(Player& player, float deltaTime);
	void updateLoadedChunks(ChunkCoord& playerCoord);
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
	std::queue<Chunk*> chunksToLoad;

	float sinceLoadedChunk;
	bool isFirstTimeLoading;
	bool isGenerating;

	int shaderProgram;
	int shaderModelLoc;

	std::vector<BlockMod> blocksToGenerate;
	TextureSheet* sheet;
	BlockPalette* pallete;
	FastNoiseLite perlinNoise;
	ChunkCoord lastPlayerChunkCoord;
};