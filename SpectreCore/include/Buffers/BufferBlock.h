#pragma once
#include "SpectreApi.h"
#include "VulkanBuffer.h"
BEGIN_NAMESPACE_SPECTRE

class BufferBlock
{
public:
	void Alloc(uint32_t size);

	void PushBuffer(void* pBuffer, uint32_t size);

	void Upload(VkBufferUsageFlagBits usage);

	VulkanBufferPtr GetDeviceBuffer() { return m_DeviceBufferPtr; }
private:
	uint32_t m_UseSize;
	uint32_t m_Capacity;
	VulkanBufferPtr m_StageBufferPtr;
	VulkanBufferPtr m_DeviceBufferPtr;
};

END_NAMESPACE_SPECTRE