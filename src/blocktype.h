#pragma once

struct BlockType
{
	unsigned char id;

	int frontFaceTexture;
	int rightFaceTexture;
	int backFaceTexture;
	int leftFaceTexture;
	int topFaceTexture;
	int bottomFaceTexture;
	
	bool isSolid;
};