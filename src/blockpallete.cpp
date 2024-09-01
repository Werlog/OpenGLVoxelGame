#include "blockpallete.h"

void BlockPalette::registerBlocks()
{
	BlockType air = BlockType{ 0, 0, 0, 0, 0, 0, 0, false };
	BlockType grass = BlockType{ 1, 0, 0, 0, 0, 1, 9, true };
	BlockType dirt = BlockType{ 2, 9, 9, 9, 9, 9, 9, true };
	BlockType stone = BlockType{ 3, 5, 5, 5, 5, 5, 5, true };
	BlockType bedrock = BlockType{ 4, 8, 8, 8, 8, 8, 8, true };

	pallete.insert({ air.id, air });
	pallete.insert({ grass.id, grass });
	pallete.insert({ dirt.id, dirt });
	pallete.insert({ stone.id, stone });
	pallete.insert({ bedrock.id, bedrock });
}

BlockType BlockPalette::get(unsigned char blockId)
{
	if (pallete.count(blockId) != 0)
		return pallete[blockId];
	else
		return pallete[0];
}