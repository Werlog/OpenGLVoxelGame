#pragma once

#include "splinednoise.h"

class SplinedGenerator
{
public:

	SplinedGenerator();

	void setSeed(int seed);

	float get2DSplinedNoise(float x, float y);
	float get3DNoise(float x, float y, float z);
	float getBasicNoise(float x, float y);
private:
	SplinedNoise basicNoise;
	SplinedNoise mountainNoise;
};