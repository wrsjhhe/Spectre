#pragma once
#include <memory>
#include "RenderTypes.h"
#include "SpectreApi.h"
#include "VulkanCommon.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanBuffer;
typedef std::shared_ptr<VulkanBuffer> VulkanBufferPtr;

class VulkanBuffer:public SpectreApi
{
public:
	DefineClass(VulkanBuffer);
public:
	static VulkanBufferPtr Create(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);

public:

	~VulkanBuffer();

	VkDeviceMemory& GetVkDeviceMemory() { return m_VkMemory; }

	VkBuffer& GetVkBuffer() { return m_VkbBuffer; }

	uint64_t GetSize()const { return m_Size; }

	void CopyTo(VulkanBufferPtr dstBuffer, const VkCommandBuffer& commandBuffer);

	void Map(void* ptr,bool keepMap = false);

	void UnMap();

	void Flush();

	void Destroy();

private:
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);
public:
	void* MapPointerCache = nullptr;

private:
	uint32_t                                     m_Size = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE