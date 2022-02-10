#pragma once
#include "VulkanPhysicalDevice.h"
#include "VulkanQueue.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanDevice : public std::enable_shared_from_this<VulkanDevice>,public Noncopyable
{
public:
	using ExtensionFeatures = VulkanPhysicalDevice::ExtensionFeatures;

	static std::shared_ptr<VulkanDevice> Create(const VulkanPhysicalDevice& PhysicalDevice);

public:

	~VulkanDevice();
	std::shared_ptr<VulkanDevice> GetSharedPtr()
	{
		return shared_from_this();
	}

	std::shared_ptr<const VulkanDevice> GetSharedPtr() const
	{
		return shared_from_this();
	}
	const VulkanPhysicalDevice& GetPhysicalDevice() const
	{
		return m_PhysicalDevice;
	}

	VkDevice GetVkDevice() const
	{
		return m_VkDevice;
	}

	//VkQueue GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
	const VulkanQueue& GetGraphicQueue() const { return m_GraphicQueue; }
	const VulkanQueue& GetTransferQueue() const { return m_TransferQueue; }
	const VulkanQueue& GetComputeQueue() const { return m_ComputeQueue; }

private:
	VulkanDevice(const VulkanPhysicalDevice& PhysicalDevice);

private:
	const VulkanPhysicalDevice&		   m_PhysicalDevice;
	VkDevice                           m_VkDevice = VK_NULL_HANDLE;
	std::vector<VkPipelineStageFlags>  m_SupportedStagesMask;
	std::vector<VkAccessFlags>         m_SupportedAccessMask;
	VulkanQueue                        m_GraphicQueue;
	VulkanQueue                        m_ComputeQueue;
	VulkanQueue                        m_TransferQueue;
};

END_NAMESPACE_SPECTRE