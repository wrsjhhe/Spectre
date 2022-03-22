#pragma once
#include "SpectreApi.h"
#include "VulkanBuffer.h"
#include <map>
BEGIN_NAMESPACE_SPECTRE

class BufferBlock
{
public:
	BufferBlock();

	void Alloc(uint32_t size);

	void Relloc(uint32_t size);

	void PushBuffer(void* pBuffer, uint32_t size);

	void Upload(VkBufferUsageFlagBits usage);

	VulkanBufferPtr GetDeviceBuffer() { return m_DeviceBufferPtr; }

	void Free();

private:
	uint32_t m_UsedSize = 0;
	uint32_t m_Capacity = 0;
	uint32_t m_CurrentSizeLevel = 0;
	void*	 m_DataPtr = nullptr;
	//VulkanBufferPtr m_StageBufferPtr = nullptr;
	VulkanBufferPtr m_DeviceBufferPtr = nullptr;
};

END_NAMESPACE_SPECTRE