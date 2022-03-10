#include "VulkanCommon.h"
#include "VulkanPipeline.h"
#include "VulkanPrimitive.h"

USING_NAMESPACE(Spectre)

void VulkanPrimitive::BindDrawCmd(const VkCommandBuffer& cmdBuffer)
{
	VkDeviceSize offsets[1] = { 0 };
	vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &VertexBufferPtr->GetVkBuffer(), offsets);
	vkCmdBindIndexBuffer(cmdBuffer, IndicesBufferPtr->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
	vkCmdDrawIndexed(cmdBuffer, IndicesCount, 1, 0, 0, 0);
}
