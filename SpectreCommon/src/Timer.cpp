#include "Timer.h"

USING_NAMESPACE(Spectre)

template <typename T>
T getElapsedTime(std::chrono::high_resolution_clock::time_point StartTime)
{
	auto CurrTime = std::chrono::high_resolution_clock::now();
	auto time_span = std::chrono::duration_cast<std::chrono::duration<T>>(CurrTime - StartTime);
	return time_span.count();
}

Timer::Timer()
{
	Restart();
}

void Timer::Restart()
{
	m_StartTime = std::chrono::high_resolution_clock().now();
}

double Timer::GetElapsedTime() const
{
	return getElapsedTime<double>(m_StartTime);
}

float Timer::GetElapsedTimef() const
{
	return getElapsedTime<float>(m_StartTime);
}
