#include "codetimer.h"
#include <iostream>

CodeTimer::CodeTimer(const std::string& taskName)
{
	this->taskName = taskName;
	startTimePoint = std::chrono::high_resolution_clock::now();
}

CodeTimer::~CodeTimer()
{
	Stop();
}

void CodeTimer::Stop()
{
	auto endTimePoint = std::chrono::high_resolution_clock::now();

	auto start = std::chrono::time_point_cast<std::chrono::microseconds>(startTimePoint).time_since_epoch().count();
	auto end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimePoint).time_since_epoch().count();

	auto duration = end - start;

	double milliseconds = duration * 0.001;

	std::cout << taskName << " Finished, time elapsed: " << milliseconds << " ms." << std::endl;
}