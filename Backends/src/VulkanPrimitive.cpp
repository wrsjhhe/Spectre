#include "VulkanCommon.h"
#include "VulkanPipeline.h"
#include "VulkanPrimitive.h"

USING_NAMESPACE(Spectre)

void VulkanPrimitive::BindDrawCmd(const VkCommandBuffer& cmdBuffer)
{
	VkDeviceSize offsets[1] = { 0 };
	VertexBufferPtr->CmdBind(cmdBuffer, offsets);
	IndicesBufferPtr->CmdBind(cmdBuffer);
	vkCmdDrawIndexed(cmdBuffer, IndicesCount, 1, 0, 0, 0);
}
