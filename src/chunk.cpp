#include "chunk.h"
#include "world.h"
#include <glad/glad.h>
#include "voxeldata.h"
#include <vector>
#include <iostream>
#include <thread>
#include "profiling/codetimer.h"

Chunk::Chunk(BlockPalette* worldPallete, World* world, ChunkCoord position, SplinedGenerator* generator)
{
	this->worldPallete = worldPallete;
	this->position = position;
	this->generator = generator;
	this->world = world;
	this->modified = false;
	this->generated.store(false);
	this->readyToUpdate.store(true);


	std::memset(blocks, 0, sizeof(blocks));

	VAO = 0;
	EBO = 0;
	VBO = 0;
	indicesCount = 0;
}

Chunk::~Chunk()
{
	if (VBO != 0)
	{
		glDeleteBuffers(1, &VBO);
	}
	if (EBO != 0)
	{
		glDeleteBuffers(1, &EBO);
	}
	if (VAO != 0)
	{
		glDeleteVertexArrays(1, &VAO);
	}
}

void Chunk::doGenerateChunk()
{
	CodeTimer genTimer = CodeTimer("Chunk Generation");
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				blocks[x][y][z] = getGenerateBlockAt(*generator, (position.x * CHUNK_SIZE_X) + x, y, (position.y * CHUNK_SIZE_Z) + z);
			}
		}
	}

	
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int z = 0; z < CHUNK_SIZE_Z; z++)
		{
			float tree = generator->getBasicNoise((position.x * CHUNK_SIZE_X + x) * 30.0f, (position.y * CHUNK_SIZE_Z + z) * 30.0f);
			if (tree > 0.74f)
			{
				float heightMod = generator->get2DSplinedNoise((x + position.x * CHUNK_SIZE_X) * heightNoiseScale, (z + position.y * CHUNK_SIZE_Z) * heightNoiseScale) * heightNoiseMultiplier;
				int height = terrainHeight + heightMod;
				if (blocks[x][height][z] == 0) continue;
				float treeHeight = (1 - tree) * 35.0f;
				if (treeHeight < 5.0f) treeHeight = 5.0f;
				world->addBlockMods(generateTree(position.x * CHUNK_SIZE_X + x, height + 1, position.y * CHUNK_SIZE_Z + z, treeHeight));
			}
		}
	}
	
	generated.store(true);
}

void Chunk::generateChunk()
{
	std::thread generateThread = std::thread(&Chunk::doGenerateChunk, this);

	generateThread.detach();
}

unsigned char Chunk::getBlockAt(int x, int y, int z)
{
	if (x > CHUNK_SIZE_X - 1 || x < 0 || y > CHUNK_SIZE_Y - 1 || y < 0 || z > CHUNK_SIZE_Z - 1 || z < 0) 
	{
		return world->getBlockAt((position.x * CHUNK_SIZE_X) + x, y, (position.y * CHUNK_SIZE_Z) + z, true);
	}
	return blocks[x][y][z];
}

void Chunk::setBlockAt(int x, int y, int z, unsigned char blockType)
{
	blocks[x][y][z] = blockType;
}

void Chunk::setBlockAtDontUpdate(int x, int y, int z, unsigned char blockType)
{
	blocks[x][y][z] = blockType;
}

std::vector<BlockMod> Chunk::generateTree(int xPos, int yPos, int zPos, int height)
{
	std::vector<BlockMod> tree = std::vector<BlockMod>();
	tree.reserve(height + (height - 5) * 2);

	for (int y = 0; y < height; y++)
	{
		if (y < height - 1)
			blocks[xPos - position.x * CHUNK_SIZE_X][yPos + y][zPos - position.y * CHUNK_SIZE_Z] = 5;

		if (y > height - 5)
		{
			int size = 3;
			if (y >= height - 4) size = 3;
			if (y >= height - 2) size = 2;
			if (y == height - 1) size = 1;
			for (int x = -size; x <= size; x++)
			{
				for (int z = -size; z <= size; z++)
				{
					if (x == 0 && z == 0 && y != height - 1) continue;

					int chunkX = xPos + x - position.x * CHUNK_SIZE_X;
					int chunkZ = zPos + z - position.y * CHUNK_SIZE_Z;

					if (chunkX > -1 && chunkX < CHUNK_SIZE_X && chunkZ > -1 && chunkZ < CHUNK_SIZE_Z)
					{
						blocks[chunkX][yPos + y][chunkZ] = 8;
						continue;
					}

					tree.push_back(BlockMod{xPos + x, yPos + y, zPos + z, 8});
				}
			}
		}
	}

	return tree;
}

unsigned char Chunk::getGenerateBlockAt(SplinedGenerator& noise, int x, int y, int z)
{
	float heightMod = noise.get2DSplinedNoise(x * heightNoiseScale, z * heightNoiseScale) * heightNoiseMultiplier;
	int height = terrainHeight + heightMod;

	float caveMultiplier = ((terrainHeight - y) / (float)terrainHeight);
	if (caveMultiplier < 0.75f) caveMultiplier = 0.75f;
	float cavesMod = noise.get3DNoise(x * 3.0f, y * 7.0f, z * 3.0f) * 1.0f;
	if (cavesMod > 0.4f && y > 2)
	{
		return 0;
	}

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

void Chunk::updateMesh(TextureSheet& sheet)
{
	readyToUpdate.store(false);
	std::thread updateThread = std::thread(&Chunk::doUpdateMesh, this, sheet);
	updateThread.detach();
}

void Chunk::doUpdateMesh(TextureSheet& textureSheet)
{
	isUpdating.store(true);
	vertexData.clear();
	indices.clear();
	int curIndex = 0;
	for (int x = 0; x < CHUNK_SIZE_X; x++)
	{
		for (int y = 0; y < CHUNK_SIZE_Y; y++)
		{
			for (int z = 0; z < CHUNK_SIZE_Z; z++)
			{
				BlockType currentBlock = worldPallete->get(blocks[x][y][z]);
				if (currentBlock.id == 0) {
					continue;
				}
				int madeFaces = 0;
				int discardedFaces = 0;

				for (int faceIndex = 0; faceIndex < 6; faceIndex++)
				{
					std::lock_guard lock(world->chunksMutex);

					int checkIndex = faceIndex * 3;
					int checkX = faceChecks[checkIndex] + x;
					int checkY = faceChecks[checkIndex + 1] + y;
					int checkZ = faceChecks[checkIndex + 2] + z;

					unsigned char block = getBlockAt(checkX, checkY, checkZ);
					BlockType check = worldPallete->get(block);
					if (check.isSolid)
					{
						discardedFaces++;
						continue;
					}
					
					std::vector<float> uvs = textureSheet.getUVs(getTextureNumberFromFaceIndex(currentBlock, faceIndex));

					int uvCounter = 0;
					for (size_t i = faceIndex * 12; i < (faceIndex + 1) * 12; i += 3)
					{
						int posX = voxelVerts[i] + x;
						int posY = voxelVerts[i + 1] + y;
						int posZ = voxelVerts[i + 2] + z + 1;

						vertexData.push_back(ChunkVertex{ posX | posY << 5 | posZ << 14 | faceIndex << 19, uvs[uvCounter], uvs[uvCounter + 1]});

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

	readyToUpdate.store(true);
	isUpdating.store(false);
}

void Chunk::createMesh()
{
	if (!readyToUpdate.load()) return;
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
	
	glBufferData(GL_ARRAY_BUFFER, vertexData.size() * sizeof(ChunkVertex), vertexData.data(), GL_DYNAMIC_DRAW);
	glVertexAttribIPointer(0, 1, GL_INT, sizeof(ChunkVertex), 0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ChunkVertex), (void*)sizeof(int));
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