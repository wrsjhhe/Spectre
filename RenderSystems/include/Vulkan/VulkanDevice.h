#pragma once
#include "VulkanPhysicalDevice.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanDevice : public std::enable_shared_from_this<VulkanDevice>
{
public:
	using ExtensionFeatures = VulkanPhysicalDevice::ExtensionFeatures;

	static std::shared_ptr<VulkanDevice> Create(const VulkanPhysicalDevice& PhysicalDevice,
		const VkDeviceCreateInfo& DeviceCI,
		const ExtensionFeatures& EnabledExtFeatures,
		const VkAllocationCallbacks* vkAllocator);

public:
	VulkanDevice(const VulkanDevice&) = delete;
	VulkanDevice(VulkanDevice&&) = delete;
	VulkanDevice& operator = (const VulkanDevice&) = delete;
	VulkanDevice& operator = (VulkanDevice&&) = delete;

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
		return m_VulkanPhysicalDevice; 
	}

	VkDevice GetVkDevice() const
	{
		return m_VkDevice;
	}

	VkQueue GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;
	VkQueue GetGraphicQueue() const { return m_GraphicQueue; }
	VkQueue GetTransferQueue() const { return m_TransferQueue; }
	VkQueue GetComputeQueue() const { return m_ComputeQueue; }

private:
	VulkanDevice(const VulkanPhysicalDevice& PhysicalDevice,
		const VkDeviceCreateInfo& DeviceCI,
		const ExtensionFeatures& EnabledExtFeatures,
		const VkAllocationCallbacks* vkAllocator);

private:
	const VulkanPhysicalDevice&        m_VulkanPhysicalDevice;
	VkDevice                           m_VkDevice = VK_NULL_HANDLE;
	const VkAllocationCallbacks* const m_VkAllocator;
	const VkPhysicalDeviceFeatures     m_EnabledFeatures;
	ExtensionFeatures                  m_EnabledExtFeatures = {};
	std::vector<VkPipelineStageFlags>  m_SupportedStagesMask;
	std::vector<VkAccessFlags>         m_SupportedAccessMask;
	VkQueue                            m_GraphicQueue = VK_NULL_HANDLE;
	VkQueue                            m_ComputeQueue = VK_NULL_HANDLE;
	VkQueue                            m_TransferQueue = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE