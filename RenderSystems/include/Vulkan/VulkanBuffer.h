#pragma once

BEGIN_NAMESPACE_SPECTRE

class VulkanBuffer
{
public:
	VulkanBuffer(const std::shared_ptr<VulkanDevice>& vulkanDevice);

	VkDeviceMemory GetVkDeviceMemory()const { return m_VkMemory; }

	VkBuffer GetVkBuffer() const { return m_VkbBuffer; }

	void CreateHostBuffer(void* ptr,uint32_t size);

	void CreateDeviceVertexBuffer(uint32_t size);

	void CreateDeviceIndexBuffer(uint32_t size);

	void MapToDevice(const VulkanBuffer& dstBuffer,const VkCommandBuffer& commandBuffer);

private:
	void CreateBuffer(void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

private:
	std::shared_ptr<VulkanDevice>                m_VulkanDevicePtr;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE