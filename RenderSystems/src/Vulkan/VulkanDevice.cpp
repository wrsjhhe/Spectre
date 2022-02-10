#include <memory>
#include "VulkanCommon.h"
#include "VulkanDevice.h"

USING_NAMESPACE(Spectre)


std::shared_ptr<VulkanDevice> VulkanDevice::Create(const VulkanPhysicalDevice& PhysicalDevice)
{
	auto* LogicalDevice = new VulkanDevice{ PhysicalDevice };
	return std::shared_ptr<VulkanDevice>{LogicalDevice};
}

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& physicalDevice):
	m_PhysicalDevice(physicalDevice)
{
	const std::vector<VkQueueFamilyProperties>& queueProps = physicalDevice.GetQueueProperties();
	uint32_t queueCount = queueProps.size();

	std::vector<VkDeviceQueueCreateInfo> deviceQueueCI(queueCount);


	//一般的显卡只有一种图形队列，RTX显卡有另外两种专用队列
	uint32_t numPriorities = 0;
	int32_t graphicsQueueIndex = -1;
	int32_t computeQueueIndex = -1;
	int32_t transferQueueIndex = -1;
	for (int i = 0; i < queueCount; ++i)
	{
		VkDeviceQueueCreateInfo& queueCI = deviceQueueCI[i];
		queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCI.flags = 0;
		queueCI.queueCount = queueProps[i].queueCount;
		//queueCI.pQueuePriorities = &queuePriorities[i];
		if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			graphicsQueueIndex = physicalDevice.FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
			queueCI.queueFamilyIndex = graphicsQueueIndex;
		}
		else if ( !(queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
				  (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
		{
			computeQueueIndex = physicalDevice.FindQueueFamily(VK_QUEUE_COMPUTE_BIT);
			queueCI.queueFamilyIndex = computeQueueIndex;
		}
		else if (!(queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
			!(queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
			(queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
		{
			transferQueueIndex = physicalDevice.FindQueueFamily(VK_QUEUE_TRANSFER_BIT);
			queueCI.queueFamilyIndex = transferQueueIndex;
		}


	}
	std::vector<std::vector<float>> queuePriorities(deviceQueueCI.size());
	for (uint32_t index = 0; index < deviceQueueCI.size(); ++index)
	{
		float queuePriority = 1.0f;
		VkDeviceQueueCreateInfo& currQueue = deviceQueueCI[index];
		queuePriorities[index] = std::vector(currQueue.queueCount,1.0f);
		currQueue.pQueuePriorities = queuePriorities[index].data();
	}

	VkDeviceCreateInfo vkDeviceCreateInfo{};
	vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.enabledLayerCount = 0;       // 已废弃
	vkDeviceCreateInfo.ppEnabledLayerNames = nullptr; // 已废弃
	vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCI.size());
	vkDeviceCreateInfo.pQueueCreateInfos = deviceQueueCI.data();

	const auto& vkDeviceFeatures = physicalDevice.GetFeatures();
	VkPhysicalDeviceFeatures vkEnabledFeatures{};
	vkDeviceCreateInfo.pEnabledFeatures = &vkEnabledFeatures;

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	if (physicalDevice.IsExtensionSupported(VK_KHR_MAINTENANCE1_EXTENSION_NAME))
		deviceExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME); // 支持反转viewport

	vkDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();
	vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

	VK_CHECK(vkCreateDevice(physicalDevice.GetVkPhysicalDevice(), &vkDeviceCreateInfo, nullptr, &m_VkDevice),
		"Failed to create logical device");

	if (graphicsQueueIndex >= 0)
	{
		m_GraphicQueue = VulkanQueue(m_VkDevice, graphicsQueueIndex);
	}
	else
	{
		std::runtime_error("None graphicQueue!");
	}

	if (computeQueueIndex >= 0)
	{
		m_ComputeQueue = VulkanQueue(m_VkDevice, computeQueueIndex);
	}
	else
	{
		m_ComputeQueue = m_GraphicQueue;
	}

	if (transferQueueIndex >= 0)
	{
		m_TransferQueue = VulkanQueue(m_VkDevice, transferQueueIndex);
	}
	else
	{
		m_TransferQueue = m_GraphicQueue;
	}
}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(m_VkDevice, nullptr);
}

//VkQueue VulkanDevice::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
//{
//	VkQueue vkQueue = VK_NULL_HANDLE;
//	vkGetDeviceQueue(m_VkDevice,
//		queueFamilyIndex, // Index of the queue family to which the queue belongs
//		0,                // Index within this queue family of the queue to retrieve
//		&vkQueue);
//	EXP_CHECK(vkQueue != VK_NULL_HANDLE,"Can not find queue");
//	return vkQueue;
//}


