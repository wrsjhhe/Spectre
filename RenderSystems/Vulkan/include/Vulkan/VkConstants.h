#pragma once
#include <stdint.h>

#if defined(NDEBUG)
#define VK_ENABLE_VALIDATION 0
#else
#define VK_ENABLE_VALIDATION 1
#endif