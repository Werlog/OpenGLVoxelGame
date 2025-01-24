#pragma once

#include "chunk.h"
#include <queue>
#include "splinedgenerator.h"
#include <mutex>

constexpr int RENDER_DISTANCE = 10;

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
private:
	std::vector<Chunk*> loadedChunks;
	std::vector<Chunk*> loadingChunks;

	SplinedGenerator splinedGenerator;

	int shaderProgram;
	int shaderModelLoc;

	std::vector<BlockMod> blocksToGenerate;
	TextureSheet* sheet;
	BlockPalette* pallete;
	ChunkCoord lastPlayerChunkCoord;
};