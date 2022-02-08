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
	VulkanBuffer(){}
	explicit VulkanBuffer(const std::shared_ptr<const VulkanDevice>& vulkanDevice);

	~VulkanBuffer();

	void SetDevice(const std::shared_ptr<const VulkanDevice>& vulkanDevice)
	{
		m_DevicePtr = vulkanDevice;
	}

	BufferType GetBufferType() const { return m_BufferType; }

	 VkDeviceMemory& GetVkDeviceMemory() { return m_VkMemory; }

	 VkBuffer& GetVkBuffer()  { return m_VkbBuffer; }

	void CreateHostBuffer(const void* ptr,uint32_t size);

	void CreateHostUniformBuffer(const void* ptr, uint32_t size);

	void CreateDeviceVertexBuffer(uint32_t size);

	void CreateDeviceIndexBuffer(uint32_t size);

	void MapToDevice(VulkanBuffer& dstBuffer,const VkCommandPool& commandPool, const VkCommandBuffer& commandBuffer);

	void UpdateHostBuffer(const void* const ptr);

	void Release();

private:
	void CreateBuffer(const void* ptr, uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

private:
	std::shared_ptr<const VulkanDevice>			 m_DevicePtr;
	BufferType                                   m_BufferType = Buffer_Type_Undefined;
	uint32_t                                     m_Size = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE