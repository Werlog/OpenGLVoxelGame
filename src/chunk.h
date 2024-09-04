#pragma once

#include "texturesheet.h"
#include "blockpallete.h"
#include "perlinNoiseLib/PerlinNoise.hpp"

constexpr int CHUNK_SIZE_X = 16;
constexpr int CHUNK_SIZE_Y = 256;
constexpr int CHUNK_SIZE_Z = 16;

struct ChunkCoord
{
	int x, y;

	bool operator==(ChunkCoord other)
	{
		return this->x == other.x && this->y == other.y;
	}

	static ChunkCoord toChunkCoord(const glm::vec3& aPos)
	{
		int x = (int)floor(aPos.x / CHUNK_SIZE_X);
		int y = (int)floor(aPos.z / CHUNK_SIZE_Z);

		return ChunkCoord{ x, y };
	}
	static ChunkCoord toChunkCoord(int xPos, int zPos)
	{
		int x = (int)floor(xPos / CHUNK_SIZE_X);
		int y = (int)floor(zPos / CHUNK_SIZE_Z);

		return ChunkCoord{ x, y };
	}
};

class World;

class Chunk
{
public:
	ChunkCoord position;
	unsigned char blocks[CHUNK_SIZE_X][CHUNK_SIZE_Y][CHUNK_SIZE_Z];

	Chunk(BlockPalette* worldPallete, World* world, ChunkCoord position, siv::PerlinNoise* noise);

	void generateChunk();
	unsigned char getBlockAt(int x, int y, int z);
	void setBlockAt(int x, int y, int z, unsigned char blockType, TextureSheet& updateSheet);
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

	void createMesh(std::vector<float>& vertexData, std::vector<int>& indices);
	int getTextureNumberFromFaceIndex(BlockType& block, int faceIndex);
};