#pragma once

#include "chunk.h"

constexpr int WORLD_SIZE_X = 15;
constexpr int WORLD_SIZE_Z = 15;

class World
{
public:
	World(BlockPalette* pallete, TextureSheet* sheet, int shaderHandle);
	~World();

	void createWorld();
	void renderWorld();
	void modifyBlockAt(int x, int y, int z, unsigned char newBlockType);
	unsigned char getBlockAt(int x, int y, int z, bool includeNotGenerated);
	unsigned char getBlockAt(float x, float y, float z);
	Chunk* getChunkByCoordinate(ChunkCoord coord);
private:
	int shaderProgram;
	int shaderModelLoc;

	std::vector<Chunk*> chunks;
	TextureSheet* sheet;
	BlockPalette* pallete;
	siv::PerlinNoise perlinNoise;
};