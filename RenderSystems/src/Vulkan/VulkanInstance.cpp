#include <stdexcept>
#include <vector>
#include "VulkanCommon.h"
#include "VulkanInstance.h"



USING_NAMESPACE(Spectre)
USING_NAMESPACE(RenderSystems)

bool VulkanInstance::IsExtensionAvailable(const std::vector<VkExtensionProperties>& extensions, const char* ExtensionName) const
{
    for (const auto& Extension : extensions)
        if (strcmp(Extension.extensionName, ExtensionName) == 0)
            return true;

    return false;
}

VulkanInstance::VulkanInstance(const CreateInfo& CI):
	m_pVkAllocator{ CI.pVkAllocator }
{
    //获取所有vulkan可用扩展
    uint32_t ExtensionCount = 0;
    std::vector<VkExtensionProperties> extensions;
    auto res = vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, nullptr);
    VK_CHECK(res, "Failed to create logical device!");
    extensions.resize(ExtensionCount);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &ExtensionCount, extensions.data());
    VK_CHECK(res, "Failed to enumerate extensions");

    std::vector<const char*> InstanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(SPE_PLATFORM_WINDOWS)      
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#if defined(SPE_PLATFORM_ANDROID)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
    };

    if (IsExtensionAvailable(extensions,VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        InstanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (CI.ppInstanceExtensionNames != nullptr)
    {
        for (uint32_t i = 0; i < CI.InstanceExtensionCount; ++i)
            InstanceExtensions.push_back(CI.ppInstanceExtensionNames[i]);
    }
    else
    {
        if (CI.InstanceExtensionCount != 0)
        {
            /*LOG_ERROR_MESSAGE("Global extensions pointer is null while extensions count is ", CI.InstanceExtensionCount,
                ". Please initialize 'ppInstanceExtensionNames' member of EngineVkCreateInfo struct.");*/
        }
    }

    for (const auto* ExtName : InstanceExtensions)
    {
        if (!IsExtensionAvailable(extensions,ExtName))
            throw std::runtime_error( std::string("Required extension " )+ ExtName + " is not available");
    }

    auto ApiVersion = CI.ApiVersion;
    if (vkEnumerateInstanceVersion != nullptr && ApiVersion > VK_API_VERSION_1_0)
    {
        uint32_t MaxApiVersion = 0;
        vkEnumerateInstanceVersion(&MaxApiVersion);
        ApiVersion = std::min(ApiVersion, MaxApiVersion);
    }
    else
    {
        // Only Vulkan 1.0 is supported.
        ApiVersion = VK_API_VERSION_1_0;
    }
}