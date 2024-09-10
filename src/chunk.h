#pragma once

#include "texturesheet.h"
#include "blockpallete.h"
#include "perlinNoiseLib/PerlinNoise.hpp"

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

constexpr int terrainHeight = 45;
constexpr double heightNoiseScale = 0.03;
constexpr double heightNoiseMultiplier = 25;

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
	bool modified;
	unsigned char blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	Chunk(BlockPalette* worldPallete, World* world, ChunkCoord position, siv::PerlinNoise* noise);
	~Chunk();

	void generateChunk();
	unsigned char getBlockAt(int x, int y, int z);
	void setBlockAt(int x, int y, int z, unsigned char blockType, TextureSheet& updateSheet);
	void setBlockAtDontUpdate(int x, int y, int z, unsigned char blockType);
	std::vector<BlockMod> generateTree(int x, int y, int z, int height);
	static unsigned char getGenerateBlockAt(siv::PerlinNoise& noise, int x, int y, int z); // Returns the block that should be generated at that coordinate

	void updateMesh(TextureSheet& sheet);
	void renderChunk();
private:
	unsigned int VAO;
	unsigned int EBO;
	unsigned int VBO;
	BlockPalette* worldPallete;
	siv::PerlinNoise* noise;
	World* world;


	int indicesCount;

	void createMesh(std::vector<ChunkVertex>& vertexData, std::vector<int>& indices);
	int getTextureNumberFromFaceIndex(BlockType& block, int faceIndex);
};