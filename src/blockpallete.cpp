#include "blockpallete.h"

void BlockPalette::registerBlocks()
{
	BlockType air = BlockType{ 0, 0, 0, 0, 0, 0, 0, false, true };
	BlockType grass = BlockType{ 1, 0, 0, 0, 0, 1, 9, true, false };
	BlockType dirt = BlockType{ 2, 9, 9, 9, 9, 9, 9, true, false };
	BlockType stone = BlockType{ 3, 5, 5, 5, 5, 5, 5, true, false };
	BlockType bedrock = BlockType{ 4, 8, 8, 8, 8, 8, 8, true, false };
	BlockType oaklog = BlockType{ 5, 2, 2, 2, 2, 3, 3, true, false };
	BlockType cobblestone = BlockType{ 6, 6, 6, 6, 6, 6, 6, true, false };
	BlockType oakplanks = BlockType{ 7, 7, 7, 7, 7, 7, 7, true, false };
	BlockType leaves = BlockType{ 8, 4, 4, 4, 4, 4, 4, true, true };

	pallete.insert({ air.id, air });
	pallete.insert({ grass.id, grass });
	pallete.insert({ dirt.id, dirt });
	pallete.insert({ stone.id, stone });
	pallete.insert({ bedrock.id, bedrock });
	pallete.insert({ oaklog.id, oaklog });
	pallete.insert({ cobblestone.id, cobblestone });
	pallete.insert({ oakplanks.id, oakplanks });
	pallete.insert({ leaves.id, leaves });
}

const BlockType& BlockPalette::get(unsigned char blockId)
{
	if (pallete.count(blockId) != 0)
		return pallete[blockId];
	else
		return pallete[0];
}