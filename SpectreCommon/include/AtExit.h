#pragma once
#include "SpectreDef.h"
#include <algorithm>

BEGIN_NAMESPACE_SPECTRE


class AtExit
{
public:
	AtExit();

	static void AddExitFunc(int order, std::function<void()> func);

	static void Execute();
private:

	static std::vector<	std::pair<int, std::function<void()>>> m_ExitFuncArr;
};



END_NAMESPACE_SPECTRE