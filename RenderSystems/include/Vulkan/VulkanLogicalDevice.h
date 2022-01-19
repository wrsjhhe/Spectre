#pragma once
#include "VulkanPhysicalDevice.h"

BEGIN_NAMESPACE_RENDERSYSTEMS

class VulkanLogicalDevice
{
public:
	using ExtensionFeatures = VulkanPhysicalDevice::ExtensionFeatures;

	static std::shared_ptr<VulkanLogicalDevice> Create(const VulkanPhysicalDevice& PhysicalDevice,
		const VkDeviceCreateInfo& DeviceCI,
		const ExtensionFeatures& EnabledExtFeatures,
		const VkAllocationCallbacks* vkAllocator);

public:
	VulkanLogicalDevice(const VulkanLogicalDevice&) = delete;
	VulkanLogicalDevice(VulkanLogicalDevice&&) = delete;
	VulkanLogicalDevice& operator = (const VulkanLogicalDevice&) = delete;
	VulkanLogicalDevice& operator = (VulkanLogicalDevice&&) = delete;

	~VulkanLogicalDevice();

private:
	VulkanLogicalDevice(const VulkanPhysicalDevice& PhysicalDevice,
		const VkDeviceCreateInfo& DeviceCI,
		const ExtensionFeatures& EnabledExtFeatures,
		const VkAllocationCallbacks* vkAllocator);

private:
	VkDevice                           m_VkDevice = VK_NULL_HANDLE;
	const VkAllocationCallbacks* const m_VkAllocator;
	const VkPhysicalDeviceFeatures     m_EnabledFeatures;
	ExtensionFeatures                  m_EnabledExtFeatures = {};
	std::vector<VkPipelineStageFlags>  m_SupportedStagesMask;
	std::vector<VkAccessFlags>         m_SupportedAccessMask;
};

END_NAMESPACE_RENDERSYSTEMS