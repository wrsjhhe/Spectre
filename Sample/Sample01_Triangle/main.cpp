#include "Vulkan/RenderSystemVK.h"

using namespace Spectre;

int main()
{
	RenderSystemVK rsvk;
	rsvk.Init();
	rsvk.Loop();
	//while (true)
	//{
	//	rsvk.Loop();
	//}

	return 0;
}