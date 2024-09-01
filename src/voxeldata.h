#pragma once

constexpr float voxelVerts[] =
{
	// Front Face
	0.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,

	// Right Face
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, -1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, -1.0f,

	// Back Face
	1.0f, 0.0f, -1.0f,
	0.0f, 0.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	0.0f, 1.0f, -1.0f,

	// Left Face
	0.0f, 0.0f, -1.0f,
	0.0f, 0.0f, 0.0f,
	0.0f, 1.0f, -1.0f,
	0.0f, 1.0f, 0.0f,

	// Top Face
	0.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,

	// Bottom Face
	0.0f, 0.0f, 0.0f,
	0.0f, 0.0f, -1.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, -1.0f,
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