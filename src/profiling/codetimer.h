#pragma once

#include <chrono>
#include <string>

class CodeTimer
{
public:
	CodeTimer(const std::string& taskName);

	~CodeTimer();

	void Stop();
private:
	std::string taskName;

	std::chrono::time_point<std::chrono::high_resolution_clock> startTimePoint;
};