#include "splinednoise.h"
#include <iostream>

SplinedNoise::SplinedNoise()
{
	noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);

	scale = 1.0f;
}

void SplinedNoise::setSeed(int seed)
{
	noise.SetSeed(seed);
}

void SplinedNoise::setScale(float scale)
{
	this->scale = scale;
}

float SplinedNoise::get2DSplinedNoise(float x, float y)
{
	float noiseVal = noise.GetNoise(x * scale, y * scale);

	Spline lower = getLowerSpline(noiseVal);
	Spline higher = getHigherSpline(noiseVal);

	float interpolated = lower.outVal + ((noiseVal - lower.noiseVal) / (higher.noiseVal - lower.noiseVal)) * (higher.outVal - lower.outVal);

	if (higher.noiseVal - lower.noiseVal == 0.0f)
	{
		interpolated = lower.outVal;
	}

	return interpolated;
}

void SplinedNoise::addSpline(float noiseVal, float outputVal)
{
	splines.push_back(Spline{noiseVal, outputVal});
}

FastNoiseLite& SplinedNoise::getNoise()
{
	return noise;
}

Spline SplinedNoise::getLowerSpline(float val)
{
	for (int i = 1; i < splines.size(); i++)
	{
		Spline& curSpline = splines[i];
		if (curSpline.noiseVal > val)
		{
			return splines[i - 1];
		}
	}

	return Spline{0.0f, 0.0f};
}

Spline SplinedNoise::getHigherSpline(float val)
{
	for (Spline& spline : splines)
	{
		if (spline.noiseVal >= val) return spline;
	}

	return Spline{0.0f, 0.0f};
}