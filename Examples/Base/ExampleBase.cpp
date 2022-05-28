#include "ExampleBase.h"
#include <iomanip>
#include <sstream>

const double filterScale = 0.2;

void ExampleBase::InitFPS()
{
	m_LastTime = m_timer.GetElapsedTime();
}

void ExampleBase::UpdateFPS(std::string& strFPS, double& elapsedTime)
{
	double currTime = m_timer.GetElapsedTime();
	elapsedTime = currTime - m_LastTime;

	m_FilteredFrameTime = m_FilteredFrameTime * (1.0 - filterScale) + filterScale * elapsedTime;
	std::stringstream fpsCounterSS;
	fpsCounterSS << "Sample02_TextureBox -----";
	fpsCounterSS << std::fixed << std::setprecision(1) << m_FilteredFrameTime * 1000;
	fpsCounterSS << " ms (" << 1.0 / m_FilteredFrameTime << " fps)";
	strFPS = fpsCounterSS.str();

	m_LastTime = currTime;
	m_CurrTime = m_CurrTime + elapsedTime;
}

