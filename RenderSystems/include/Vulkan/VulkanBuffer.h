#pragma once
#include <memory>
#include "VulkanGraphicTypes.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;

class VulkanBuffer
{
public:

public:
	static std::shared_ptr<VulkanBuffer> Create(const VulkanDevice& vulkanDevice, uint32_t size, 
		VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags, void* data = nullptr);

public:
	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer(VulkanBuffer&&) = delete;
	VulkanBuffer& operator = (const VulkanBuffer&) = delete;
	VulkanBuffer& operator = (VulkanBuffer&&) = delete;

	~VulkanBuffer();

	VkDeviceMemory& GetVkDeviceMemory() { return m_VkMemory; }

	VkBuffer& GetVkBuffer() { return m_VkbBuffer; }


	void CopyTo(VulkanBuffer& dstBuffer, const VkCommandBuffer& commandBuffer);

	VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void UnMap();

	void UpdateHostBuffer(const void* const ptr);

	VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0);

	void Destroy();

private:
	explicit VulkanBuffer(const VulkanDevice& vulkanDevice);
	void CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

private:
	const VulkanDevice&							 m_Device;
	uint32_t                                     m_Size = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
	void*										 m_DataPtr;
};

END_NAMESPACE_SPECTRE