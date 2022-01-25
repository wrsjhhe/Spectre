#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "RenderSystemVK.h"


USING_NAMESPACE(Spectre)

void RenderSystemVK::Init()
{
	VulkanInstance::CreateInfo instanceCI;
#ifdef VKB_DEBUG
	instanceCI.EnableValidation = true;
#endif

	std::shared_ptr<VulkanInstance> instance = VulkanInstance::Create(instanceCI);

}
