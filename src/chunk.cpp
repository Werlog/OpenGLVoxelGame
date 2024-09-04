#include "chunk.h"
#include "world.h"
#include <glad/glad.h>
#include "voxeldata.h"
#include <vector>
#include <iostream>

Chunk::Chunk(BlockPalette* worldPallete, World* world, ChunkCoord position, siv::PerlinNoise* noise)
{
	this->worldPallete = worldPallete;
	this->position = position;
	this->noise = noise;
	this->world = world;

	std::memset(blocks, 0, sizeof(blocks));

	VAO = 0;
	EBO = 0;
	VBO = 0;
	indicesCount = 0;
}

void Chunk::generateChunk()
{
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				blocks[x][y][z] = getGenerateBlockAt(*noise, (position.x * CHUNK_SIZE_X) + x, y, (position.y * CHUNK_SIZE_Z) + z);
			}
		}
	}
}

unsigned char Chunk::getBlockAt(int x, int y, int z)
{
	if (x > CHUNK_SIZE_X - 1 || x < 0 || y > CHUNK_SIZE_Y - 1 || y < 0 || z > CHUNK_SIZE_Z - 1 || z < 0) 
	{
		return world->getBlockAt((position.x * CHUNK_SIZE_X) + x, y, (position.y * CHUNK_SIZE_Z) + z, true);
	}
	return blocks[x][y][z];
}

void Chunk::setBlockAt(int x, int y, int z, unsigned char blockType, TextureSheet& updateSheet)
{
	blocks[x][y][z] = blockType;
	updateMesh(updateSheet);
}

unsigned char Chunk::getGenerateBlockAt(siv::PerlinNoise& noise, int x, int y, int z)
{
	int terrainHeight = 45;
	float heightMod = noise.octave2D_01((double)x * 0.03, (double)z * 0.03, 2) * 35;
	int height = terrainHeight + heightMod;

	if (y > height)
	{
		return 0;
	}

	if (y == height)
	{
		return 1;
	}
	else if (y >= height - 2)
	{
		return 2;
	}
	else if (y < height - 2)
	{
		if (y != 0) 
		{
			return 3;
		}

		return 4;
	}
}

void Chunk::updateMesh(TextureSheet& textureSheet)
{
	std::vector<float> vertexData;
	std::vector<int> indices;
	int curIndex = 0;
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				BlockType currentBlock = worldPallete->get(blocks[x][y][z]);
				if (currentBlock.id == 0) continue; // Air

				int madeFaces = 0;
				int discardedFaces = 0;
				for (int faceIndex = 0; faceIndex < 6; faceIndex++)
				{
					int checkIndex = faceIndex * 3;
					int checkX = faceChecks[checkIndex] + x;
					int checkY = faceChecks[checkIndex + 1] + y;
					int checkZ = faceChecks[checkIndex + 2] + z;
					
					if (getBlockAt(checkX, checkY, checkZ) != 0)
					{
						discardedFaces++;
						continue;
					}

					std::vector<float> uvs = textureSheet.getUVs(getTextureNumberFromFaceIndex(currentBlock, faceIndex));


					int uvCounter = 0;
					for (size_t i = faceIndex * 12; i < (faceIndex + 1) * 12; i += 3)
					{
						vertexData.push_back(voxelVerts[i] + x);
						vertexData.push_back(voxelVerts[i + 1] + y);
						vertexData.push_back(voxelVerts[i + 2] + z);
						vertexData.push_back(uvs[uvCounter]);
						vertexData.push_back(uvs[uvCounter + 1]);

						uvCounter += 2;
					}

					for (size_t i = faceIndex * 6; i < (faceIndex + 1) * 6; i++)
					{
						indices.push_back(voxelIndices[i - discardedFaces * 6] + curIndex);
					}

					madeFaces++;
				}

				curIndex += madeFaces * 4;
			}
		}
	}

	indicesCount = indices.size();

	createMesh(vertexData, indices);
}

void Chunk::createMesh(std::vector<float>& vertexData, std::vector<int>& indices)
{
	if (VAO == 0)
	{
		glGenVertexArrays(1, &VAO);
	}
	glBindVertexArray(VAO);

	if (EBO == 0)
	{
		glGenBuffers(1, &EBO);
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);

	if (VBO == 0)
	{
		glGenBuffers(1, &VBO);
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(float), vertexData.data(), GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void Chunk::renderChunk()
{
	if (VAO == 0) return;

	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	glDrawElements(GL_TRIANGLES, indicesCount, GL_UNSIGNED_INT, 0);
}

int Chunk::getTextureNumberFromFaceIndex(BlockType& block, int faceIndex)
{
	switch (faceIndex)
	{
	case 0:
		return block.frontFaceTexture;
	case 1:
		return block.rightFaceTexture;
	case 2:
		return block.backFaceTexture;
	case 3:
		return block.leftFaceTexture;
	case 4:
		return block.topFaceTexture;
	case 5:
		return block.bottomFaceTexture;
	default:
		return block.frontFaceTexture;
	}
}