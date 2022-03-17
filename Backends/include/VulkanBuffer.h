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

	uint64_t GetTotalSize()const { return m_TotalSize; }

	void CopyTo(VulkanBufferPtr dstBuffer);

	void Map(void* ptr,uint32_t size ,uint32_t offset,  bool keepMap = false);

	void UnMap();

	void Flush();

	void Destroy();

private:
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);
public:
	void* MapPointerCache = nullptr;

private:
	uint32_t                                     m_TotalSize = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkbBuffer = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE