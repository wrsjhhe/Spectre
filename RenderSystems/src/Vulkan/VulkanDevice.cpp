#include <memory>
#include "VulkanCommon.h"
#include "VulkanDevice.h"

USING_NAMESPACE(Spectre)


std::shared_ptr<VulkanDevice> VulkanDevice::Create(const VulkanPhysicalDevice& PhysicalDevice,
	const VkDeviceCreateInfo& DeviceCI, 
	const ExtensionFeatures& EnabledExtFeatures, 
	const VkAllocationCallbacks* vkAllocator)
{
	auto* LogicalDevice = new VulkanDevice{ PhysicalDevice, DeviceCI, EnabledExtFeatures, vkAllocator };
	return std::shared_ptr<VulkanDevice>{LogicalDevice};
}

VulkanDevice::VulkanDevice(const VulkanPhysicalDevice& PhysicalDevice, 
	const VkDeviceCreateInfo& DeviceCI, 
	const ExtensionFeatures& EnabledExtFeatures, 
	const VkAllocationCallbacks* vkAllocator):
	m_VulkanPhysicalDevice(PhysicalDevice),
	m_VkAllocator{ vkAllocator },
	m_EnabledFeatures{ *DeviceCI.pEnabledFeatures },
	m_EnabledExtFeatures{ EnabledExtFeatures }
{
	VkPhysicalDevice physicalDevice = m_VulkanPhysicalDevice.GetVkPhysicalDevice();
	auto res = vkCreateDevice(physicalDevice, &DeviceCI, vkAllocator, &m_VkDevice);
	VK_CHECK(res, "Failed to create logical device");

	auto GraphicsStages =
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
		VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT |
		VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
		VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT |
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT |
		VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
	auto ComputeStages =
		VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT |
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

	auto GraphicsAccessMask =
		VK_ACCESS_INDEX_READ_BIT |
		VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT |
		VK_ACCESS_INPUT_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
		VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
		VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	auto ComputeAccessMask =
		VK_ACCESS_INDIRECT_COMMAND_READ_BIT |
		VK_ACCESS_UNIFORM_READ_BIT |
		VK_ACCESS_SHADER_READ_BIT |
		VK_ACCESS_SHADER_WRITE_BIT;
	auto TransferAccessMask =
		VK_ACCESS_TRANSFER_READ_BIT |
		VK_ACCESS_TRANSFER_WRITE_BIT |
		VK_ACCESS_HOST_READ_BIT |
		VK_ACCESS_HOST_WRITE_BIT;

	if (DeviceCI.pEnabledFeatures->geometryShader)
		GraphicsStages |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
	if (DeviceCI.pEnabledFeatures->tessellationShader)
		GraphicsStages |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
	if (m_EnabledExtFeatures.MeshShader.meshShader != VK_FALSE && m_EnabledExtFeatures.MeshShader.taskShader != VK_FALSE)
		GraphicsStages |= VK_PIPELINE_STAGE_TASK_SHADER_BIT_NV | VK_PIPELINE_STAGE_MESH_SHADER_BIT_NV;
	if (m_EnabledExtFeatures.RayTracingPipeline.rayTracingPipeline != VK_FALSE)
	{
		ComputeStages |= VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR | VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR;
		ComputeAccessMask |= VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;
	}
	if (m_EnabledExtFeatures.ShadingRate.attachmentFragmentShadingRate != VK_FALSE)
	{
		GraphicsStages |= VK_PIPELINE_STAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		GraphicsAccessMask |= VK_ACCESS_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
	}
	if (m_EnabledExtFeatures.FragmentDensityMap.fragmentDensityMap != VK_FALSE)
	{
		GraphicsStages |= VK_PIPELINE_STAGE_FRAGMENT_DENSITY_PROCESS_BIT_EXT;
		GraphicsAccessMask |= VK_ACCESS_FRAGMENT_DENSITY_MAP_READ_BIT_EXT;
	}

	const auto QueueCount = m_VulkanPhysicalDevice.GetQueueProperties().size();
	m_SupportedStagesMask.resize(QueueCount, 0);
	m_SupportedAccessMask.resize(QueueCount, 0);
	for (size_t q = 0; q < QueueCount; ++q)
	{
		const auto& Queue = m_VulkanPhysicalDevice.GetQueueProperties()[q];
		auto& StageMask = m_SupportedStagesMask[q];
		auto& AccessMask = m_SupportedAccessMask[q];

		if (Queue.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			vkGetDeviceQueue(m_VkDevice, q, 0, &m_GraphicQueue);
			vkGetDeviceQueue(m_VkDevice, q, 0, &m_ComputeQueue);
			vkGetDeviceQueue(m_VkDevice, q, 0, &m_TransferQueue);
			StageMask |= GraphicsStages | ComputeStages | VK_PIPELINE_STAGE_ALL_TRANSFER;
			AccessMask |= GraphicsAccessMask | ComputeAccessMask | TransferAccessMask;
		}
		else if (Queue.queueFlags & VK_QUEUE_COMPUTE_BIT)
		{
			if (m_ComputeQueue == VK_NULL_HANDLE)
			{
				vkGetDeviceQueue(m_VkDevice, q, 0, &m_ComputeQueue);
			}
			if (m_TransferQueue == VK_NULL_HANDLE)
			{
				vkGetDeviceQueue(m_VkDevice, q, 0, &m_TransferQueue);
			}
	
			StageMask |= ComputeStages | VK_PIPELINE_STAGE_ALL_TRANSFER;
			AccessMask |= ComputeAccessMask | TransferAccessMask;
		}
		else if (Queue.queueFlags & VK_QUEUE_TRANSFER_BIT)
		{
			if (m_TransferQueue == VK_NULL_HANDLE)
			{
				vkGetDeviceQueue(m_VkDevice, q, 0, &m_TransferQueue);
			}
			StageMask |= VK_PIPELINE_STAGE_ALL_TRANSFER;
			AccessMask |= TransferAccessMask;
		}
	}

}

VulkanDevice::~VulkanDevice()
{
	vkDestroyDevice(m_VkDevice, m_VkAllocator);
}

VkQueue VulkanDevice::GetQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const
{
	VkQueue vkQueue = VK_NULL_HANDLE;
	vkGetDeviceQueue(m_VkDevice,
		queueFamilyIndex, // Index of the queue family to which the queue belongs
		0,                // Index within this queue family of the queue to retrieve
		&vkQueue);
	EXP_CHECK(vkQueue != VK_NULL_HANDLE,"Can not find queue");
	return vkQueue;
}


