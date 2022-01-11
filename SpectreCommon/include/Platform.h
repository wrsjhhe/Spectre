#pragma once

#if defined(_WIN32) || defined(_WIN64)
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#	define SPE_PLATFORM_WINDOWS 1
#elif defined(__linux__)
#	define SPE_PLATFORM_LINUX 1
#elif defined(__ANDROID__)
#	define SPE_PLATFORM_ANDROID 1
#else
#	error "SPE_PLATFORM_* is not defined!"
#endif