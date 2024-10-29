#include "splinedgenerator.h"
#include <iostream>

SplinedGenerator::SplinedGenerator()
{
	basicNoise.setScale(1.5f);
	mountainNoise.setScale(0.2f);
	valleyNoise.setScale(0.2f);

	basicNoise.addSpline(-1.0f, -0.5f);
	basicNoise.addSpline(1.0f, 0.5f);

	mountainNoise.addSpline(-1.0f, 0.0f);
	mountainNoise.addSpline(0.3f, 0.0f);
	mountainNoise.addSpline(1.0f, 1.0f);

	valleyNoise.addSpline(-1.0f, 0.3f);
	valleyNoise.addSpline(0.2f, 0.3f);
	valleyNoise.addSpline(1.0f, -1.0f);
}

void SplinedGenerator::setSeed(int seed)
{
	basicNoise.setSeed(seed);
	mountainNoise.setSeed(seed - 1);
	valleyNoise.setSeed(seed + 1);
}

float SplinedGenerator::get2DSplinedNoise(float x, float y)
{
	float basic = basicNoise.get2DSplinedNoise(x, y);
	float mountain = mountainNoise.get2DSplinedNoise(x, y);
	float valley = valleyNoise.get2DSplinedNoise(x, y);

	return (mountain * 8.0f + basic * 2.2f + valley * 4.4f) / 2.0f;
}

float SplinedGenerator::get3DNoise(float x, float y, float z)
{
	return basicNoise.getNoise().GetNoise(x, y, z);
}

float SplinedGenerator::getBasicNoise(float x, float y)
{
	return basicNoise.getNoise().GetNoise(x, y);
}
