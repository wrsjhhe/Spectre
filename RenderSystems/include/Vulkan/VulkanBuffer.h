#pragma once
#include <memory>


BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;

class VulkanBuffer
{
public:

public:
	static std::shared_ptr<VulkanBuffer> Create(uint32_t size, 
		VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags,const void* data = nullptr);

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
	explicit VulkanBuffer();
	void CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

private:
	uint32_t                                     m_Size = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
	void*										 m_DataPtr = nullptr;
};

END_NAMESPACE_SPECTRE