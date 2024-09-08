#pragma once

constexpr int voxelVerts[] =
{
	// Front Face
	0, 0, 0,
	1, 0, 0,
	0, 1, 0,
	1, 1, 0,

	// Right Face
	1, 0, 0,
	1, 0, -1,
	1, 1, 0,
	1, 1, -1,

	// Back Face
	1, 0, -1,
	0, 0, -1,
	1, 1, -1,
	0, 1, -1,

	// Left Face
	0, 0, -1,
	0, 0, 0,
	0, 1, -1,
	0, 1, 0,

	// Top Face
	0, 1, 0,
	1, 1, 0,
	0, 1, -1,
	1, 1, -1,

	// Bottom Face
	0, 0, 0,
	0, 0, -1,
	1, 0, 0,
	1, 0, -1,
};
constexpr int voxelIndices[] = 
{
	// Front Face
	0, 1, 2,
	1, 3, 2,

	// Right Face
	4, 5, 6,
	5, 7, 6,

	// Back Face
	8, 9, 10,
	9, 11, 10,

	// Left Face
	12, 13, 14,
	13, 15, 14,

	// Top Face
	16, 17, 18,
	17, 19, 18,

	// Bottom Face
	20, 21, 22,
	21, 23, 22,
};

constexpr int faceChecks[] =
{
	// Front Face
	0, 0, 1,

	// Right Face
	1, 0, 0,

	// Back Face
	0, 0, -1,

	// Left Face
	-1, 0, 0,

	// Top Face
	0, 1, 0,

	// Bottom Face
	0, -1, 0,
};