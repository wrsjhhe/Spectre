#pragma once
#include <vector>
#include <memory>

BEGIN_NAMESPACE_SPECTRE

class VulkanInstance : public std::enable_shared_from_this<VulkanInstance>
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

	std::shared_ptr<VulkanInstance> GetSharedPtr()
	{
		return shared_from_this();
	}

	std::shared_ptr<const VulkanInstance> GetSharedPtr() const
	{
		return shared_from_this();
	}

	bool IsExtensionEnabled(const char* ExtensionName)const;

	const std::vector<VkPhysicalDevice>& GetVkPhysicalDevices() const { return m_PhysicalDevices; }
	VkAllocationCallbacks* GetVkAllocator() const { return m_pVkAllocator; }
	VkInstance             GetVkInstance()  const { return m_VkInstance; }

private:
	explicit VulkanInstance(const CreateInfo& CI);
private:
	bool							   m_DebugUtilsEnabled = false;

	VkInstance						   m_VkInstance = VK_NULL_HANDLE;
	VkAllocationCallbacks* const	   m_pVkAllocator;
	std::vector<const char*>           m_EnabledExtensions;
	std::vector<VkPhysicalDevice>      m_PhysicalDevices;

	VkDebugUtilsMessengerEXT debug_utils_messenger{ VK_NULL_HANDLE };
};

END_NAMESPACE_SPECTRE