#include <vector>
#include <memory>
#include "VulkanCommon.h"
#include "VulkanPhysicalDevice.h"


USING_NAMESPACE(Spectre)



bool IsExtensionSupported(const std::vector<VkExtensionProperties>& supportedExtensions,const char* extensionName)
{
	for (const auto& Extension : supportedExtensions)
		if (strcmp(Extension.extensionName, extensionName) == 0)
			return true;

	return false;
}

std::unique_ptr<VulkanPhysicalDevice> VulkanPhysicalDevice::Create(VkPhysicalDevice vkDevice, const VulkanInstance& Instance)
{
	auto* PhysicalDevice = new VulkanPhysicalDevice{ vkDevice, Instance };
	return std::unique_ptr<VulkanPhysicalDevice>{PhysicalDevice};
}

VulkanPhysicalDevice::VulkanPhysicalDevice(VkPhysicalDevice vkDevice, const VulkanInstance& instance):
	m_VkPhysicalDevice(vkDevice)
{
	EXP_CHECK(m_VkPhysicalDevice != VK_NULL_HANDLE,"Physical device is null")

	VkPhysicalDeviceProperties properties = {};
	VkPhysicalDeviceFeatures   features = {};

	vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &properties);
	vkGetPhysicalDeviceFeatures(m_VkPhysicalDevice, &features);
	vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &m_MemoryProperties);
	uint32_t QueueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, nullptr);
	m_QueueFamilyProperties.resize(QueueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, m_QueueFamilyProperties.data());


	// 获取显卡支持的扩展
	uint32_t ExtensionCount = 0;
	std::vector<VkExtensionProperties>   supportedExtensions;
	vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &ExtensionCount, nullptr);
	if (ExtensionCount > 0)
	{
		supportedExtensions.resize(ExtensionCount);
		auto res = vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &ExtensionCount, supportedExtensions.data());
		VK_CHECK(res,"Failed get device extension properties");
	}

	ExtensionFeatures extFeatures = {};
	ExtensionProperties extProperties = {};
	if (instance.IsExtensionEnabled(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
	{
		VkPhysicalDeviceFeatures2 Feats2{};
		Feats2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		void** NextFeat = &Feats2.pNext;

		VkPhysicalDeviceProperties2 Props2{};
		Props2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		void** NextProp = &Props2.pNext;

		if (IsExtensionSupported(supportedExtensions,VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.ShaderFloat16Int8;
			NextFeat = &extFeatures.ShaderFloat16Int8.pNext;

			extFeatures.ShaderFloat16Int8.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_FLOAT16_INT8_FEATURES_KHR;
		}

		// VK_KHR_16bit_storage and VK_KHR_8bit_storage extensions require VK_KHR_storage_buffer_storage_class extension.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_STORAGE_BUFFER_STORAGE_CLASS_EXTENSION_NAME))
		{
			if (IsExtensionSupported(supportedExtensions, VK_KHR_16BIT_STORAGE_EXTENSION_NAME))
			{
				*NextFeat = &extFeatures.Storage16Bit;
				NextFeat = &extFeatures.Storage16Bit.pNext;

				extFeatures.Storage16Bit.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_16BIT_STORAGE_FEATURES;
			}

			if (IsExtensionSupported(supportedExtensions, VK_KHR_8BIT_STORAGE_EXTENSION_NAME))
			{
				*NextFeat = &extFeatures.Storage8Bit;
				NextFeat = &extFeatures.Storage8Bit.pNext;

				extFeatures.Storage8Bit.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_8BIT_STORAGE_FEATURES;
			}
		}

		// Get mesh shader features and properties.
		if (IsExtensionSupported(supportedExtensions, VK_NV_MESH_SHADER_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.MeshShader;
			NextFeat = &extFeatures.MeshShader.pNext;

			extFeatures.MeshShader.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_FEATURES_NV;

			*NextProp = &extProperties.MeshShader;
			NextProp = &extProperties.MeshShader.pNext;

			extProperties.MeshShader.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MESH_SHADER_PROPERTIES_NV;
		}

		// Get acceleration structure features and properties.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.AccelStruct;
			NextFeat = &extFeatures.AccelStruct.pNext;

			extFeatures.AccelStruct.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;

			*NextProp = &extProperties.AccelStruct;
			NextProp = &extProperties.AccelStruct.pNext;

			extProperties.AccelStruct.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_PROPERTIES_KHR;
		}

		// Get ray tracing pipeline features and properties.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.RayTracingPipeline;
			NextFeat = &extFeatures.RayTracingPipeline.pNext;

			extFeatures.RayTracingPipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;

			*NextProp = &extProperties.RayTracingPipeline;
			NextProp = &extProperties.RayTracingPipeline.pNext;

			extProperties.RayTracingPipeline.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_PROPERTIES_KHR;
		}

		// Get inline ray tracing features.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_RAY_QUERY_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.RayQuery;
			NextFeat = &extFeatures.RayQuery.pNext;

			extFeatures.RayQuery.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_QUERY_FEATURES_KHR;
		}

		// Additional extension that is required for ray tracing.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.BufferDeviceAddress;
			NextFeat = &extFeatures.BufferDeviceAddress.pNext;

			extFeatures.BufferDeviceAddress.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES_KHR;
		}

		// Additional extension that is required for ray tracing.
		if (IsExtensionSupported(supportedExtensions, VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.DescriptorIndexing;
			NextFeat = &extFeatures.DescriptorIndexing.pNext;

			extFeatures.DescriptorIndexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;

			*NextProp = &extProperties.DescriptorIndexing;
			NextProp = &extProperties.DescriptorIndexing.pNext;

			extProperties.DescriptorIndexing.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_PROPERTIES_EXT;
		}

		// Additional extension that is required for ray tracing shader.
		if (IsExtensionSupported(supportedExtensions, VK_KHR_SPIRV_1_4_EXTENSION_NAME))
			extFeatures.Spirv14 = true;

		// Some features require SPIRV 1.4 or 1.5 which was added to the Vulkan 1.2 core.
		if (g_VkVersion >= VK_API_VERSION_1_2)
		{
			extFeatures.Spirv14 = true;
			extFeatures.Spirv15 = true;
		}

		// Extension required for MoltenVk
		if (IsExtensionSupported(supportedExtensions, VK_KHR_PORTABILITY_SUBSET_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.PortabilitySubset;
			NextFeat = &extFeatures.PortabilitySubset.pNext;

			extFeatures.HasPortabilitySubset = true;
			extFeatures.PortabilitySubset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_FEATURES_KHR;

			*NextProp = &extProperties.PortabilitySubset;
			NextProp = &extProperties.PortabilitySubset.pNext;

			extProperties.PortabilitySubset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PORTABILITY_SUBSET_PROPERTIES_KHR;
		}

		// Subgroup feature requires Vulkan 1.1 core.
		if (g_VkVersion >= VK_API_VERSION_1_1)
		{
			*NextProp = &extProperties.Subgroup;
			NextProp = &extProperties.Subgroup.pNext;

			extFeatures.SubgroupOps = true;
			extProperties.Subgroup.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SUBGROUP_PROPERTIES;
		}

		if (IsExtensionSupported(supportedExtensions, VK_EXT_VERTEX_ATTRIBUTE_DIVISOR_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.VertexAttributeDivisor;
			NextFeat = &extFeatures.VertexAttributeDivisor.pNext;

			extFeatures.VertexAttributeDivisor.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_FEATURES_EXT;

			*NextProp = &extProperties.VertexAttributeDivisor;
			NextProp = &extProperties.VertexAttributeDivisor.pNext;

			extProperties.VertexAttributeDivisor.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VERTEX_ATTRIBUTE_DIVISOR_PROPERTIES_EXT;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.TimelineSemaphore;
			NextFeat = &extFeatures.TimelineSemaphore.pNext;

			extFeatures.TimelineSemaphore.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_FEATURES;

			*NextProp = &extProperties.TimelineSemaphore;
			NextProp = &extProperties.TimelineSemaphore.pNext;

			extProperties.TimelineSemaphore.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_TIMELINE_SEMAPHORE_PROPERTIES;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_MULTIVIEW_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.Multiview;
			NextFeat = &extFeatures.Multiview.pNext;

			extFeatures.Multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_FEATURES_KHR;

			*NextProp = &extProperties.Multiview;
			NextProp = &extProperties.Multiview.pNext;

			extProperties.Multiview.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MULTIVIEW_PROPERTIES_KHR;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME))
		{
			extFeatures.RenderPass2 = true;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_FRAGMENT_SHADING_RATE_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.ShadingRate;
			NextFeat = &extFeatures.ShadingRate.pNext;

			extFeatures.ShadingRate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_FEATURES_KHR;

			*NextProp = &extProperties.ShadingRate;
			NextProp = &extProperties.ShadingRate.pNext;

			extProperties.ShadingRate.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_SHADING_RATE_PROPERTIES_KHR;
		}

		if (IsExtensionSupported(supportedExtensions, VK_EXT_FRAGMENT_DENSITY_MAP_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.FragmentDensityMap;
			NextFeat = &extFeatures.FragmentDensityMap.pNext;

			extFeatures.FragmentDensityMap.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_FEATURES_EXT;

			*NextProp = &extProperties.FragmentDensityMap;
			NextProp = &extProperties.FragmentDensityMap.pNext;

			extProperties.FragmentDensityMap.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FRAGMENT_DENSITY_MAP_PROPERTIES_EXT;
		}

		if (IsExtensionSupported(supportedExtensions, VK_EXT_HOST_QUERY_RESET_EXTENSION_NAME))
		{
			*NextFeat = &extFeatures.HostQueryReset;
			NextFeat = &extFeatures.HostQueryReset.pNext;

			extFeatures.HostQueryReset.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_HOST_QUERY_RESET_FEATURES;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_DRAW_INDIRECT_COUNT_EXTENSION_NAME))
		{
			extFeatures.DrawIndirectCount = true;
		}

		if (IsExtensionSupported(supportedExtensions, VK_KHR_MAINTENANCE3_EXTENSION_NAME))
		{
			*NextProp = &extProperties.Maintenance3;
			NextProp = &extProperties.Maintenance3.pNext;

			extProperties.Maintenance3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MAINTENANCE_3_PROPERTIES;
		}

		// make sure that last pNext is null
		*NextFeat = nullptr;
		*NextProp = nullptr;

		// Initialize device extension features by current physical device features.
		// Some flags may not be supported by hardware.
		vkGetPhysicalDeviceFeatures2(m_VkPhysicalDevice, &Feats2);
		vkGetPhysicalDeviceProperties2(m_VkPhysicalDevice, &Props2);


		// Check texture formats
		if (extFeatures.ShadingRate.attachmentFragmentShadingRate != VK_FALSE)
		{
			// For compatibility with D3D12, shading rate texture must support R8_UINT format
			VkFormatProperties FmtProps{};
			vkGetPhysicalDeviceFormatProperties(m_VkPhysicalDevice, VK_FORMAT_R8_UINT, &FmtProps);

			// Disable feature if image format is not supported
			if (!(FmtProps.optimalTilingFeatures & VK_FORMAT_FEATURE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR))
			{
				extFeatures.ShadingRate = {};
				extProperties.ShadingRate = {};
			}
		}
		if (extFeatures.FragmentDensityMap.fragmentDensityMap != VK_FALSE)
		{
			VkFormatProperties FmtProps{};
			vkGetPhysicalDeviceFormatProperties(m_VkPhysicalDevice, VK_FORMAT_R8G8_UNORM, &FmtProps);

			// Disable feature if image format is not supported
			if (!(FmtProps.optimalTilingFeatures & VK_FORMAT_FEATURE_FRAGMENT_DENSITY_MAP_BIT_EXT))
			{
				extFeatures.FragmentDensityMap = {};
				extProperties.FragmentDensityMap = {};
				extFeatures.FragmentDensityMap2 = {};
				extProperties.FragmentDensityMap2 = {};
			}
		}
	}
}

uint32_t VulkanPhysicalDevice::GetMemoryTypeIndex(uint32_t memoryTypeBitsRequirement,
	VkMemoryPropertyFlags requiredProperties) const
{
	for (uint32_t memoryIndex = 0; memoryIndex < m_MemoryProperties.memoryTypeCount; memoryIndex++)
	{
		const uint32_t memoryTypeBit = (1 << memoryIndex);
		const bool     isRequiredMemoryType = (memoryTypeBitsRequirement & memoryTypeBit) != 0;
		if (isRequiredMemoryType)
		{
			const VkMemoryPropertyFlags properties = m_MemoryProperties.memoryTypes[memoryIndex].propertyFlags;
			const bool                  hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

			if (hasRequiredProperties)
				return memoryIndex;
		}
	}
	return InvalidMemoryTypeIndex;
}