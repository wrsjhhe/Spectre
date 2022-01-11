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

private:
	explicit VulkanInstance(const CreateInfo& CI);

	bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& extensions, const char* ExtensionName) const;
private:
	VkInstance						m_VkInstance = VK_NULL_HANDLE;
	VkAllocationCallbacks* const	m_pVkAllocator;

};

END_NAMESPACE_RENDERSYSTEMS