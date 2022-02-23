#pragma once
#include "SpectreDefs.h"
#include <chrono>
BEGIN_NAMESPACE_SPECTRE

class Timer
{
public:
	Timer();
	void   Restart();
	double GetElapsedTime() const;
	float  GetElapsedTimef() const;

private:
	std::chrono::high_resolution_clock::time_point m_StartTime;
};

END_NAMESPACE_SPECTRE
