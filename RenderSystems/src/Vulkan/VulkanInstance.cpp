#include <stdexcept>
#include "VulkanCommon.h"
#include "VulkanInstance.h"

USING_NAMESPACE(Spectre)


static constexpr const char* ValidationLayerNames[] =
{
    "VK_LAYER_KHRONOS_validation"
};

VKAPI_ATTR VkBool32 VKAPI_CALL debug_utils_messenger_callback(VkDebugUtilsMessageSeverityFlagBitsEXT message_severity, VkDebugUtilsMessageTypeFlagsEXT message_type,
    const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
    void* user_data)
{
    // Log debug messge
    if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        //LOGW("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
    }
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        //LOGE("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
    }
    return VK_FALSE;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT /*type*/,
    uint64_t /*object*/, size_t /*location*/, int32_t /*message_code*/,
    const char* layer_prefix, const char* message, void* /*user_data*/)
{
    if (flags & VK_DEBUG_REPORT_ERROR_BIT_EXT)
    {
        //LOGE("{}: {}", layer_prefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_WARNING_BIT_EXT)
    {
        //LOGW("{}: {}", layer_prefix, message);
    }
    else if (flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT)
    {
        //LOGW("{}: {}", layer_prefix, message);
    }
    else
    {
        //LOGI("{}: {}", layer_prefix, message);
    }
    return VK_FALSE;
}

static bool IsExtensionAvailable(const std::vector<VkExtensionProperties>& extensions, const char* ExtensionName)
{
    for (const auto& Extension : extensions)
        if (strcmp(Extension.extensionName, ExtensionName) == 0)
            return true;

    return false;
}
static bool IsLayerAvailable(const std::vector<VkLayerProperties> layers, const char* LayerName, uint32_t& Version)
{
    for (const auto& Layer : layers)
    {
        if (strcmp(Layer.layerName, LayerName) == 0)
        {
            Version = Layer.specVersion;
            return true;
        }
    }
    return false;
}


std::shared_ptr<VulkanInstance> VulkanInstance::Create(const CreateInfo& CI)
{
	auto Instance = new VulkanInstance{ CI };
	return std::shared_ptr<VulkanInstance>{Instance};
}

VulkanInstance::VulkanInstance(const CreateInfo& CI):
	m_PVkAllocator{ CI.pVkAllocator }
{

    //获取所有vulkan可用层
    uint32_t LayerCount = 0;
    auto res = vkEnumerateInstanceLayerProperties(&LayerCount, nullptr);
    VK_CHECK(res, "Failed to query layer count");
    std::vector<VkLayerProperties> layers;
    layers.resize(LayerCount);
    res = vkEnumerateInstanceLayerProperties(&LayerCount, layers.data());
    VK_CHECK(res, "Failed to enumerate extensions");


    //获取所有vulkan可用扩展
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> available_instance_extensions;
    res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
    VK_CHECK(res, "Failed to query extension count");
    available_instance_extensions.resize(extensionCount);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, available_instance_extensions.data());
    VK_CHECK(res, "Failed to enumerate extensions");


    // Todo... if headless,use VK_EXT_HEADLESS_SURFACE_EXTENSION_NAME
    std::vector<const char*> instanceExtensions =
    {
        VK_KHR_SURFACE_EXTENSION_NAME,
#if defined(SPE_PLATFORM_WINDOWS)      
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
#if defined(SPE_PLATFORM_ANDROID)
        VK_KHR_ANDROID_SURFACE_EXTENSION_NAME
#endif
    };

    if (IsExtensionAvailable(available_instance_extensions,VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (CI.ppInstanceExtensionNames != nullptr)
    {
        for (uint32_t i = 0; i < CI.InstanceExtensionCount; ++i)
            instanceExtensions.push_back(CI.ppInstanceExtensionNames[i]);
    }
    else
    {
        if (CI.InstanceExtensionCount != 0)
        {
            /*LOG_ERROR_MESSAGE("Global extensions pointer is null while extensions count is ", CI.InstanceExtensionCount,
                ". Please initialize 'ppInstanceExtensionNames' member of EngineVkCreateInfo struct.");*/
        }
    }

    for (const auto* ExtName : instanceExtensions)
    {
        if (!IsExtensionAvailable(available_instance_extensions,ExtName))
            throw std::runtime_error( std::string("Required extension " )+ ExtName + " is not available");
    }
    bool debug_utils = false;
    if (CI.EnableValidation)
    {
        m_DebugUtilsEnabled = IsExtensionAvailable(available_instance_extensions,VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        if (m_DebugUtilsEnabled)
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            debug_utils = true;
        }
        else
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
            //LOG_WARNING_MESSAGE("Extension ", VK_EXT_DEBUG_UTILS_EXTENSION_NAME, " is not available.");
        }
    }

    auto ApiVersion = CI.ApiVersion;
    if (vkEnumerateInstanceVersion != nullptr && ApiVersion > g_VkVersion)
    {
        uint32_t MaxApiVersion = 0;
        vkEnumerateInstanceVersion(&MaxApiVersion);
        ApiVersion = std::min(ApiVersion, MaxApiVersion);
    }
    else
    {
        // Only Vulkan 1.0 is supported.
        ApiVersion = g_VkVersion;
    }

    std::vector<const char*> InstanceLayers;

    // 设备模拟，启用vulkan模拟层访问硬件设备信息，设置环境变量VK_DEVSIM_FILENAME要指向设备信息文件
    // 设备文件下载地址https://vulkan.gpuinfo.org/
    if (CI.EnableDeviceSimulation)
    {
        static const char* DeviceSimulationLayer = "VK_LAYER_LUNARG_device_simulation";

        uint32_t LayerVer = 0;
        if (IsLayerAvailable(layers, DeviceSimulationLayer, LayerVer))
        {
            InstanceLayers.push_back(DeviceSimulationLayer);
        }
    }

    //添加验证层
    if (CI.EnableValidation)
    {
        for (size_t i = 0; i < _countof(ValidationLayerNames); ++i)
        {
            const char* pLayerName = ValidationLayerNames[i];
            uint32_t LayerVer = ~0u; // Prevent warning if the layer is not found
            if (IsLayerAvailable(layers,pLayerName, LayerVer))
                InstanceLayers.push_back(pLayerName);
            //else
                //LOG_WARNING_MESSAGE("Failed to find '", pLayerName, "' layer. Validation will be disabled");
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr; 
    appInfo.pApplicationName = nullptr;
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Spectre Engine";
    appInfo.engineVersion = SPECTRE_ENGINE_VERSION; 
    appInfo.apiVersion = ApiVersion;

    VkInstanceCreateInfo InstanceCreateInfo{};
    InstanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    InstanceCreateInfo.flags = 0; 
    InstanceCreateInfo.pApplicationInfo = &appInfo;
    InstanceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
    InstanceCreateInfo.ppEnabledExtensionNames = instanceExtensions.empty() ? nullptr : instanceExtensions.data();
    InstanceCreateInfo.enabledLayerCount = static_cast<uint32_t>(InstanceLayers.size());
    InstanceCreateInfo.ppEnabledLayerNames = InstanceLayers.empty() ? nullptr : InstanceLayers.data();

    //启动验证层
    VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
    VkDebugReportCallbackCreateInfoEXT debug_report_create_info = { VK_STRUCTURE_TYPE_DEBUG_REPORT_CREATE_INFO_EXT };
    if (CI.EnableValidation)
    {
        if (debug_utils)
        {
            debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
            debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
            debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;

            InstanceCreateInfo.pNext = &debug_utils_create_info;
        }
        else
        {
            debug_report_create_info.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT | VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT;
            debug_report_create_info.pfnCallback = debug_callback;

            InstanceCreateInfo.pNext = &debug_report_create_info;
        }
    }


    res = vkCreateInstance(&InstanceCreateInfo, m_PVkAllocator, &m_VkInstance);
    //CHECK_VK_ERROR_AND_THROW(res, "Failed to create Vulkan instance");

    if (CI.EnableValidation)
    {
        if (debug_utils)
        {
            res = vkCreateDebugUtilsMessengerEXT(m_VkInstance, &debug_utils_create_info, nullptr, &debug_utils_messenger);
            VK_CHECK(res, "Could not create debug utils messenger");
        }
        else
        {
            res = vkCreateDebugReportCallbackEXT(m_VkInstance, &debug_report_create_info, nullptr, &debug_report_callback);
            if (res != VK_SUCCESS)
            {
                VK_CHECK(res, "Could not create debug report callback");
            }
        }
    }

    {
        // 遍历物理设备
        uint32_t PhysicalDeviceCount = 0;
        res = vkEnumeratePhysicalDevices(m_VkInstance, &PhysicalDeviceCount, nullptr);
        VK_CHECK(res, "Failed to get physical device count");
    /*    if (PhysicalDeviceCount == 0)
            LOG_ERROR_AND_THROW("No physical devices found on the system");*/

        m_PhysicalDevices.resize(PhysicalDeviceCount);
        res = vkEnumeratePhysicalDevices(m_VkInstance, &PhysicalDeviceCount, m_PhysicalDevices.data());
        VK_CHECK(res, "Failed to enumerate physical devices");
    }
}

VulkanInstance::~VulkanInstance()
{
    if (debug_utils_messenger != VK_NULL_HANDLE)
    {
        vkDestroyDebugUtilsMessengerEXT(m_VkInstance, debug_utils_messenger, nullptr);
    }
    if (debug_report_callback != VK_NULL_HANDLE)
    {
        vkDestroyDebugReportCallbackEXT(m_VkInstance, debug_report_callback, nullptr);
    }
    vkDestroyInstance(m_VkInstance, m_PVkAllocator);
}


bool VulkanInstance::IsExtensionEnabled(const char* ExtensionName)const
{
    for (const auto& Extension : m_EnabledExtensions)
        if (strcmp(Extension, ExtensionName) == 0)
            return true;

    return false;
}