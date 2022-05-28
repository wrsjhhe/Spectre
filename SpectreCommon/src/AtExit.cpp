#include "AtExit.h"

USING_NAMESPACE(Spectre)

static bool compare(std::pair<int, std::function<void()>> a, std::pair<int, std::function<void()>> b)
{
	return a.first > b.first;
}

AtExit::AtExit()
{
	atexit(AtExit::Execute);
}

void AtExit::AddExitFunc(int order, std::function<void()> func)
{
	std::pair<int, std::function<void()>> map = std::make_pair(order, func);
	m_ExitFuncArr.push_back(map);
}

void AtExit::Execute()
{
	std::sort(m_ExitFuncArr.begin(), m_ExitFuncArr.end(), compare);
	auto iter = m_ExitFuncArr.begin();
	while (iter != m_ExitFuncArr.end())
	{
		(iter->second)();
		iter = m_ExitFuncArr.erase(iter);
	}
}

std::vector<std::pair<int, std::function<void()>>> AtExit::m_ExitFuncArr;

static AtExit _atExit;