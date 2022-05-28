#pragma once
#include <string_view>
#include "Timer.h"

using namespace Spectre;

class ExampleBase
{
protected:

	void InitFPS();
	void UpdateFPS(std::string& strFPS,double& elapsedTime);

private:
	Timer  m_timer;
	double m_CurrTime = 0.0;
	double m_LastTime = 0.0;
	double m_FilteredFrameTime = 0;
	std::string m_strFPS;
};