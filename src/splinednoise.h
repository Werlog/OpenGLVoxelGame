#pragma once

#include "FastNoiseLite.h"
#include <vector>

struct Spline
{
	float noiseVal;
	float outVal;
};
/*
	Didn't know how to really call this,
	but basically it maps different values
	of the perlin noise to different outputs
	and interpolates between them, leading
	to more interesting terrain generation.
*/
class SplinedNoise
{
public:

	SplinedNoise();

	void setSeed(int seed);
	void setScale(float scale);
	float get2DSplinedNoise(float x, float y);
	void addSpline(float noiseVal, float outputVal);
	FastNoiseLite& getNoise();
private:
	FastNoiseLite noise;
	float scale;

	Spline getLowerSpline(float val);
	Spline getHigherSpline(float val);

	std::vector<Spline> splines;
};