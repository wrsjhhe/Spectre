#pragma once
#include "VulkanInstance.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanPhysicalDevice
{
public:
	struct ExtensionFeatures
	{
		VkPhysicalDeviceMeshShaderFeaturesNV              MeshShader = {};
		VkPhysicalDevice16BitStorageFeaturesKHR           Storage16Bit = {};
		VkPhysicalDevice8BitStorageFeaturesKHR            Storage8Bit = {};
		VkPhysicalDeviceShaderFloat16Int8FeaturesKHR      ShaderFloat16Int8 = {};
		VkPhysicalDeviceAccelerationStructureFeaturesKHR  AccelStruct = {};
		VkPhysicalDeviceRayTracingPipelineFeaturesKHR     RayTracingPipeline = {};
		VkPhysicalDeviceRayQueryFeaturesKHR               RayQuery = {};
		VkPhysicalDeviceBufferDeviceAddressFeaturesKHR    BufferDeviceAddress = {};
		VkPhysicalDeviceDescriptorIndexingFeaturesEXT     DescriptorIndexing = {};
		VkPhysicalDevicePortabilitySubsetFeaturesKHR      PortabilitySubset = {};
		VkPhysicalDeviceVertexAttributeDivisorFeaturesEXT VertexAttributeDivisor = {};
		VkPhysicalDeviceTimelineSemaphoreFeaturesKHR      TimelineSemaphore = {};
		VkPhysicalDeviceHostQueryResetFeatures            HostQueryReset = {};
		VkPhysicalDeviceFragmentShadingRateFeaturesKHR    ShadingRate = {};
		VkPhysicalDeviceFragmentDensityMapFeaturesEXT     FragmentDensityMap = {}; // Only for desktop devices
		VkPhysicalDeviceFragmentDensityMap2FeaturesEXT    FragmentDensityMap2 = {}; // Only for mobile devices
		VkPhysicalDeviceMultiviewFeaturesKHR              Multiview = {}; // Required for RenderPass2

		bool Spirv14 = false; // Ray tracing requires Vulkan 1.2 or SPIRV 1.4 extension
		bool Spirv15 = false; // DXC shaders with ray tracing requires Vulkan 1.2 with SPIRV 1.5
		bool SubgroupOps = false; // Requires Vulkan 1.1
		bool HasPortabilitySubset = false;
		bool RenderPass2 = false;
		bool DrawIndirectCount = false;
	};

	struct ExtensionProperties
	{
		VkPhysicalDeviceMeshShaderPropertiesNV              MeshShader = {};
		VkPhysicalDeviceAccelerationStructurePropertiesKHR  AccelStruct = {};
		VkPhysicalDeviceRayTracingPipelinePropertiesKHR     RayTracingPipeline = {};
		VkPhysicalDeviceDescriptorIndexingPropertiesEXT     DescriptorIndexing = {};
		VkPhysicalDevicePortabilitySubsetPropertiesKHR      PortabilitySubset = {};
		VkPhysicalDeviceSubgroupProperties                  Subgroup = {};
		VkPhysicalDeviceVertexAttributeDivisorPropertiesEXT VertexAttributeDivisor = {};
		VkPhysicalDeviceTimelineSemaphorePropertiesKHR      TimelineSemaphore = {};
		VkPhysicalDeviceFragmentShadingRatePropertiesKHR    ShadingRate = {};
		VkPhysicalDeviceFragmentDensityMapPropertiesEXT     FragmentDensityMap = {};
		VkPhysicalDeviceMultiviewPropertiesKHR              Multiview = {};
		VkPhysicalDeviceMaintenance3Properties              Maintenance3 = {};
		VkPhysicalDeviceFragmentDensityMap2PropertiesEXT    FragmentDensityMap2 = {};
	};

	static constexpr uint32_t InvalidMemoryTypeIndex = ~uint32_t{ 0 };
	static std::unique_ptr<VulkanPhysicalDevice> Create(VkPhysicalDevice   vkDevice,
		const VulkanInstance& Instance);
public:
	VulkanPhysicalDevice(const VulkanPhysicalDevice&) = delete;
	VulkanPhysicalDevice(VulkanPhysicalDevice&&) = delete;
	VulkanPhysicalDevice& operator = (const VulkanPhysicalDevice&) = delete;
	VulkanPhysicalDevice& operator = (VulkanPhysicalDevice&&) = delete;

	VkPhysicalDevice GetVkDeviceHandle() const { return m_VkPhysicalDevice; }
	const std::vector<VkQueueFamilyProperties>& GetQueueProperties() const { return m_QueueFamilyProperties; }
	uint32_t GetMemoryTypeIndex(uint32_t  memoryTypeBitsRequirement,
		VkMemoryPropertyFlags requiredProperties) const;

private:
	VulkanPhysicalDevice(VkPhysicalDevice vkDevice, const VulkanInstance& Instance);

private:
	const VkPhysicalDevice               m_VkPhysicalDevice;
	VkPhysicalDeviceMemoryProperties     m_MemoryProperties = {};
	std::vector<VkQueueFamilyProperties> m_QueueFamilyProperties;
};

END_NAMESPACE_SPECTRE