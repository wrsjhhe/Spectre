#include "Vulkan/RenderSystemVK.h"

using namespace Spectre;

int main()
{
	RenderSystemVK rsvk;
	rsvk.Init();


	rsvk.Loop();
	

	return 0;
}