#pragma once
#include <memory>

BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;

class VulkanBuffer
{
public:
	enum BufferType
	{
		Buffer_Type_Undefined = -1,
		Buffer_Type_Host = 1,
		Buffer_Type_Device = 2
	};
public:

	static std::shared_ptr<VulkanBuffer> CreateHostBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice,const void* ptr, uint32_t size);

	static std::shared_ptr<VulkanBuffer> CreateHostUniformBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice, const void* ptr, uint32_t size);

	static std::shared_ptr<VulkanBuffer> CreateDeviceVertexBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice, uint32_t size);

	static std::shared_ptr<VulkanBuffer> CreateDeviceIndexBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice, uint32_t size);

public:
	VulkanBuffer(const VulkanBuffer&) = delete;
	VulkanBuffer(VulkanBuffer&&) = delete;
	VulkanBuffer& operator = (const VulkanBuffer&) = delete;
	VulkanBuffer& operator = (VulkanBuffer&&) = delete;

	~VulkanBuffer();

	BufferType GetBufferType() const { return m_BufferType; }

	VkDeviceMemory& GetVkDeviceMemory() { return m_VkMemory; }

	VkBuffer& GetVkBuffer() { return m_VkbBuffer; }


	void MapToDevice(VulkanBuffer& dstBuffer, const VkCommandPool& commandPool, const VkCommandBuffer& commandBuffer);

	void UpdateHostBuffer(const void* const ptr);

	void Release();

private:
	explicit VulkanBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice, BufferType bufferType);
	void CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

private:
	std::shared_ptr<const VulkanDevice>			 m_DevicePtr;
	BufferType                                   m_BufferType = Buffer_Type_Undefined;
	uint32_t                                     m_Size = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE