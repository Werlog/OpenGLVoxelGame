#pragma once

#include "blocktype.h"
#include <unordered_map>

class BlockPalette
{
public:
	void registerBlocks();
	const BlockType& get(unsigned char blockId);
private:
	std::unordered_map<unsigned char, BlockType> pallete;
};