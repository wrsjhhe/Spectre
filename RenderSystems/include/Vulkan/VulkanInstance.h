#pragma once
#include <memory>


BEGIN_NAMESPACE_RENDERSYSTEMS

static const uint32_t  s_VkVersion = VK_API_VERSION_1_0;

class VulkanInstance : public std::enable_shared_from_this<VkInstance>
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
public:
	VulkanInstance(const VulkanInstance&) = delete;
	VulkanInstance(VulkanInstance&&) = delete;
	VulkanInstance& operator = (const VulkanInstance&) = delete;
	VulkanInstance& operator = (VulkanInstance&&) = delete;
	~VulkanInstance();

	bool IsExtensionEnabled(const char* ExtensionName)const;
private:
	explicit VulkanInstance(const CreateInfo& CI);
private:
	bool							   m_DebugUtilsEnabled = false;

	VkInstance						   m_vkInstance = VK_NULL_HANDLE;
	VkAllocationCallbacks* const	   m_pVkAllocator;
	std::vector<const char*>           m_EnabledExtensions;
	std::vector<VkPhysicalDevice>      m_PhysicalDevices;

	VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };
	VkDebugReportCallbackEXT debug_report_callback{ VK_NULL_HANDLE };
};

END_NAMESPACE_RENDERSYSTEMS