#pragma once
#include <vector>
#include <memory>

BEGIN_NAMESPACE_SPECTRE

class VulkanInstance
{
public:
	struct CreateInfo
	{
		uint32_t               ApiVersion = 0;
		bool                   EnableValidation = false;
		bool                   EnableDeviceSimulation = false;
		uint32_t               InstanceExtensionCount = 0;
		const char* const*     ppInstanceExtensionNames = nullptr;
		VkAllocationCallbacks* pVkAllocator = nullptr;
	};
	static std::shared_ptr<VulkanInstance> Create(const CreateInfo& CI);
public:
	VulkanInstance(const VulkanInstance&) = delete;
	VulkanInstance(VulkanInstance&&) = delete;
	VulkanInstance& operator = (const VulkanInstance&) = delete;
	VulkanInstance& operator = (VulkanInstance&&) = delete;
	~VulkanInstance();

	bool IsExtensionEnabled(const char* ExtensionName)const;

	const std::vector<VkPhysicalDevice>& GetVkPhysicalDevices() const { return m_PhysicalDevices; }
private:
	explicit VulkanInstance(const CreateInfo& CI);
private:
	bool							   m_DebugUtilsEnabled = false;

	VkInstance						   m_VkInstance = VK_NULL_HANDLE;
	VkAllocationCallbacks* const	   m_PVkAllocator;
	std::vector<const char*>           m_EnabledExtensions;
	std::vector<VkPhysicalDevice>      m_PhysicalDevices;

	VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };
};

END_NAMESPACE_SPECTRE