#pragma once
#include <memory>


BEGIN_NAMESPACE_SPECTRE

class VulkanDevice;
class VulkanIndexBuffer
{
public:
	template<typename T>
	static std::shared_ptr<VulkanIndexBuffer> Create(std::vector<T> indices, VkIndexType indexType)
	{	
		auto* pIndexBuffer = new VulkanIndexBuffer(std::forward<std::vector<T>>(indices),indexType);
		return std::shared_ptr<VulkanIndexBuffer>{pIndexBuffer};
	}
private:
	VulkanIndexBuffer(std::vector<uint32_t> indices, VkIndexType indexType);
	VulkanIndexBuffer(std::vector<uint16_t> indices, VkIndexType indexType);

public:
	void Synchronize(const VkCommandBuffer& commandBuffer);

	void CmdBind(const VkCommandBuffer& commandBuffer);

	uint32_t    GetIndexCount() const { return m_IndexCount; }
	VkIndexType GetIndexType() const { return m_IndexType; }
private:
	std::shared_ptr<VulkanBuffer>   m_HostBuffer;
	std::shared_ptr<VulkanBuffer>   m_DeviceBuffer;
	uint32_t                        m_IndexCount = 0;
	VkIndexType						m_IndexType;
};

END_NAMESPACE_SPECTRE