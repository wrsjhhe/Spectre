#pragma once
#include <memory>


BEGIN_NAMESPACE_SPECTRE


class VulkanIndexBuffer;
typedef std::shared_ptr<VulkanIndexBuffer> VulkanIndexBufferPtr;
class VulkanIndexBuffer
{
public:
	template<typename T>
	static VulkanIndexBufferPtr Create(T* indices, uint32_t count, VkIndexType indexType)
	{
		auto* pIndexBuffer = new VulkanIndexBuffer(std::forward<T*>(indices), count, indexType);
		return std::shared_ptr<VulkanIndexBuffer>{pIndexBuffer};
	}
private:
	VulkanIndexBuffer(uint32_t* indices, uint32_t count , VkIndexType indexType);
	VulkanIndexBuffer(uint16_t* indices, uint32_t count , VkIndexType indexType);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer);

	VkIndexType GetIndexType() const { return m_IndexType; }
private:
	VulkanBufferPtr					m_HostBuffer;
	VulkanBufferPtr					m_DeviceBuffer;
	VkIndexType						m_IndexType;
};

END_NAMESPACE_SPECTRE