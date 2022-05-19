#pragma once
#include <memory>
#include "RenderTypes.h"
#include "SpectreApi.h"
#include "VulkanCommon.h"
#include "VulkanImage.h"
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

	VkBuffer& GetVkBuffer() { return m_VkBuffer; }

	uint64_t GetTotalSize()const { return m_TotalSize; }

	void CopyTo(VulkanBufferPtr dstBuffer);

	void CopyToImage(VulkanImagePtr dstImage, uint32_t width, uint32_t height);

	void Map(void* ptr,uint32_t size = 0,uint32_t offset = 0,  bool keepMap = false);

	void UnMap();

	void Flush(uint64_t size, uint64_t offset);

	void Destroy();

private:
	VulkanBuffer(uint32_t size, VkBufferUsageFlagBits usage, VkMemoryPropertyFlags memoryFlags);
public:
	void* MapPointerCache = nullptr;

private:
	uint32_t                                     m_TotalSize = 0;
	VkDeviceMemory				                 m_VkMemory = VK_NULL_HANDLE;
	VkBuffer					                 m_VkBuffer = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE