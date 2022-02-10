#pragma once
#include <stdint.h>
#include "NativeWindow.h"

struct EngineCreateInfo
{
	NativeWindow Wnd;
	uint32_t     Width = 0;
	uint32_t     Height = 0;

};