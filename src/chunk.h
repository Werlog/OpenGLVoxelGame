#pragma once

#include "texturesheet.h"
#include "blockpallete.h"
#include "splinedgenerator.h"
#include <atomic>

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

constexpr int terrainHeight = 65;
constexpr float heightNoiseScale = 2.00f;
constexpr float heightNoiseMultiplier = 25;

struct ChunkCoord
{
	int x, y;

	bool operator==(ChunkCoord other)
	{
		return this->x == other.x && this->y == other.y;
	}

	bool operator!=(ChunkCoord other)
	{
		return this->x != other.x || this->y != other.y;
	}

	static ChunkCoord toChunkCoord(const glm::vec3& aPos)
	{
		int x = (int)floor(aPos.x / CHUNK_SIZE_X);
		int y = (int)floor(aPos.z / CHUNK_SIZE_Z);

		return ChunkCoord{ x, y };
	}
	static ChunkCoord toChunkCoord(int xPos, int zPos)
	{
		int x = (int)floor(xPos / (float)CHUNK_SIZE_X);
		int y = (int)floor(zPos / (float)CHUNK_SIZE_Z);

		return ChunkCoord{ x, y };
	}
};

struct BlockMod
{
	int blockX, blockY, blockZ;
	unsigned char blockType;
};

struct ChunkVertex 
{
	int posData;
	float u, v;
};

class World;

class Chunk
{
public:
	ChunkCoord position;
	std::atomic<bool> generated;
	std::atomic<bool> readyToUpdate;
	std::atomic<bool> isUpdating;
	bool modified;
	unsigned char blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	Chunk(BlockPalette* worldPallete, World* world, ChunkCoord position, SplinedGenerator* generator);
	~Chunk();

	void generateChunk();
	unsigned char getBlockAt(int x, int y, int z);
	void setBlockAt(int x, int y, int z, unsigned char blockType);
	void setBlockAtDontUpdate(int x, int y, int z, unsigned char blockType);
	std::vector<BlockMod> generateTree(int x, int y, int z, int height);
	static unsigned char getGenerateBlockAt(SplinedGenerator& generator, int x, int y, int z); // Returns the block that should be generated at that coordinate
	void updateMesh(TextureSheet& sheet);

	void renderChunk();
	void createMesh();
private:
	unsigned int VAO;
	unsigned int EBO;
	unsigned int VBO;
	BlockPalette* worldPallete;
	SplinedGenerator* generator;
	World* world;
	std::vector<ChunkVertex> vertexData;
	std::vector<int> indices;

	int indicesCount;

	void doUpdateMesh(TextureSheet& sheet);
	void doGenerateChunk();
	int getTextureNumberFromFaceIndex(BlockType& block, int faceIndex);
};