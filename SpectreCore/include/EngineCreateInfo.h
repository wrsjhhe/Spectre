#pragma once
#include <stdint.h>
#include <vector>
#include "NativeWindow.h"

struct EngineCreateInfo
{
	NativeWindow Wnd;
	uint32_t     Width = 0;
	uint32_t     Height = 0;
	std::vector<std::vector<uint32_t>> VertexShaders;
	std::vector<std::vector<uint32_t>> FragmentShaders;

};