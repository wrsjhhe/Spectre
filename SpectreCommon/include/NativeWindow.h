#pragma once
#include "Platform.h"



#ifdef SPE_PLATFORM_WINDOWS

struct Win32Window
{
	void* hWnd = nullptr;

	Win32Window() noexcept
	{}

	explicit Win32Window(void* _hWnd) noexcept :
		hWnd{ _hWnd }
	{}
};

typedef struct Win32Window NativeWindow;

#endif