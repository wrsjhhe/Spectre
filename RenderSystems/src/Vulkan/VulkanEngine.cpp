#include "VulkanCommon.h"
#include "VulkanEngine.h"

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
        LOG_WARN_FMT("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
    }
    else if (message_severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        LOG_ERROR_FMT("{} - {}: {}", callback_data->messageIdNumber, callback_data->pMessageIdName, callback_data->pMessage);
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

VulkanEngine* VulkanEngine::m_EngineInstance = nullptr;

VulkanEngine* VulkanEngine::Create(const VulkanEngineCreateInfo& ci)
{
    if (m_EngineInstance == nullptr)
        m_EngineInstance = new VulkanEngine(ci);


    atexit(VulkanEngine::Destory);
    return m_EngineInstance;
}

void VulkanEngine::Destory()
{

}

VulkanEngine::VulkanEngine(const VulkanEngineCreateInfo& ci):
    m_CI(ci)
{
    CreateVkInstance();
}

void VulkanEngine::CreateVkInstance()
{
    //获取所有vulkan可用层
    uint32_t LayerCount = 0;
    VK_CHECK(vkEnumerateInstanceLayerProperties(&LayerCount, nullptr), "Failed to query layer count");
    std::vector<VkLayerProperties> layers;
    layers.resize(LayerCount);
    VK_CHECK(vkEnumerateInstanceLayerProperties(&LayerCount, layers.data()), "Failed to enumerate extensions");


    //获取所有vulkan可用扩展
    uint32_t extensionCount = 0;
    std::vector<VkExtensionProperties> available_instance_extensions;
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr), "Failed to query extension count");
    available_instance_extensions.resize(extensionCount);
    VK_CHECK(vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, available_instance_extensions.data()), "Failed to enumerate extensions");


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

    if (IsExtensionAvailable(available_instance_extensions, VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME))
    {
        instanceExtensions.push_back(VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME);
    }

    if (m_CI.ppInstanceExtensionNames != nullptr)
    {
        for (uint32_t i = 0; i < m_CI.InstanceExtensionCount; ++i)
            instanceExtensions.push_back(m_CI.ppInstanceExtensionNames[i]);
    }
    else
    {
        EXP_CHECK(m_CI.InstanceExtensionCount == 0, "Global extensions pointer is null while extensions count is ", CI.InstanceExtensionCount,
            ". Please initialize 'ppInstanceExtensionNames' member of EngineVkCreateInfo struct.");
    }

    for (const auto* ExtName : instanceExtensions)
    {
        if (!IsExtensionAvailable(available_instance_extensions, ExtName))
            throw std::runtime_error(std::string("Required extension ") + ExtName + " is not available");
    }
    bool debug_utils = false;
    if (m_CI.EnableValidation)
    {
        m_DebugUtilsEnabled = IsExtensionAvailable(available_instance_extensions, VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        if (m_DebugUtilsEnabled)
        {
            instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            debug_utils = true;
        }
    }

    auto ApiVersion = g_VkVersion;
    if (vkEnumerateInstanceVersion != nullptr)
    {
        uint32_t MaxApiVersion = 0;
        vkEnumerateInstanceVersion(&MaxApiVersion);
        ApiVersion = std::max(ApiVersion, MaxApiVersion);
    }

    std::vector<const char*> InstanceLayers;

    // 设备模拟，启用vulkan模拟层访问硬件设备信息，设置环境变量VK_DEVSIM_FILENAME要指向设备信息文件
    // 设备文件下载地址https://vulkan.gpuinfo.org/
    if (m_CI.EnableDeviceSimulation)
    {
        static const char* DeviceSimulationLayer = "VK_LAYER_LUNARG_device_simulation";

        uint32_t LayerVer = 0;
        if (IsLayerAvailable(layers, DeviceSimulationLayer, LayerVer))
        {
            InstanceLayers.push_back(DeviceSimulationLayer);
        }
    }

    //添加验证层
    if (m_CI.EnableValidation)
    {
        for (size_t i = 0; i < _countof(ValidationLayerNames); ++i)
        {
            const char* pLayerName = ValidationLayerNames[i];
            uint32_t LayerVer = ~0u; // Prevent warning if the layer is not found
            if (IsLayerAvailable(layers, pLayerName, LayerVer))
                InstanceLayers.push_back(pLayerName);
            else
                LOG_WARN("Failed to find '", pLayerName, "' layer. Validation will be disabled");
        }
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = nullptr;
    appInfo.pApplicationName = nullptr;
    appInfo.applicationVersion = SPECTRE_ENGINE_VERSION;
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

    m_EnabledExtensions = std::move(instanceExtensions);

    VK_CHECK(vkCreateInstance(&InstanceCreateInfo, nullptr, &m_VkInstance), "Failed to create Vulkan instance");

    //启动验证层
    if (debug_utils)
    {
        VkDebugUtilsMessengerCreateInfoEXT debug_utils_create_info = { VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT };
        debug_utils_create_info.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
        debug_utils_create_info.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_utils_create_info.pfnUserCallback = debug_utils_messenger_callback;

        PFN_vkCreateDebugUtilsMessengerEXT CreateDebugMessageCallback = VK_NULL_HANDLE;
        CreateDebugMessageCallback = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(m_VkInstance, "vkCreateDebugUtilsMessengerEXT");

        VK_CHECK(CreateDebugMessageCallback(m_VkInstance, &debug_utils_create_info, nullptr, &debug_utils_messenger), "Could not create debug utils messenger");
    }

}

void VulkanEngine::CreateVkPhysicalDevice()
{
    // 遍历物理设备
    uint32_t physicalDeviceCount = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, nullptr), "Failed to get physical device count");
    EXP_CHECK(physicalDeviceCount != 0, "No physical devices found on the system");
    std::vector<VkPhysicalDevice> physicalDevices;
    physicalDevices.resize(physicalDeviceCount);
    VK_CHECK(vkEnumeratePhysicalDevices(m_VkInstance, &physicalDeviceCount, physicalDevices.data()), "Failed to enumerate physical devices");

    m_VkPhysicalDevice = physicalDevices[0];
    vkGetPhysicalDeviceProperties(m_VkPhysicalDevice, &m_Properties);
    vkGetPhysicalDeviceFeatures(m_VkPhysicalDevice, &m_Features);
    vkGetPhysicalDeviceMemoryProperties(m_VkPhysicalDevice, &m_MemoryProperties);
    uint32_t QueueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, nullptr);
    m_QueueFamilyProperties.resize(QueueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(m_VkPhysicalDevice, &QueueFamilyCount, m_QueueFamilyProperties.data());


    // 获取显卡支持的扩展
    uint32_t ExtensionCount = 0;
    vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &ExtensionCount, nullptr);
    if (ExtensionCount > 0)
    {
        m_SupportedExtensions.resize(ExtensionCount);
        auto res = vkEnumerateDeviceExtensionProperties(m_VkPhysicalDevice, nullptr, &ExtensionCount, m_SupportedExtensions.data());
        VK_CHECK(res, "Failed get device extension properties");
    }
}

void VulkanEngine::CreateVkDevice()
{
    const std::vector<VkQueueFamilyProperties>& queueProps = m_QueueFamilyProperties;
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
        if (queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            graphicsQueueIndex = FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
            queueCI.queueFamilyIndex = graphicsQueueIndex;
        }
        else if (!(queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            (queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT))
        {
            computeQueueIndex = FindQueueFamily(VK_QUEUE_COMPUTE_BIT);
            queueCI.queueFamilyIndex = computeQueueIndex;
        }
        else if (!(queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
            !(queueProps[i].queueFlags & VK_QUEUE_COMPUTE_BIT) &&
            (queueProps[i].queueFlags & VK_QUEUE_TRANSFER_BIT))
        {
            transferQueueIndex = FindQueueFamily(VK_QUEUE_TRANSFER_BIT);
            queueCI.queueFamilyIndex = transferQueueIndex;
        }


    }
    std::vector<std::vector<float>> queuePriorities(deviceQueueCI.size());
    for (uint32_t index = 0; index < deviceQueueCI.size(); ++index)
    {
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo& currQueue = deviceQueueCI[index];
        queuePriorities[index] = std::vector(currQueue.queueCount, 1.0f);
        currQueue.pQueuePriorities = queuePriorities[index].data();
    }

    VkDeviceCreateInfo vkDeviceCreateInfo{};
    vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    vkDeviceCreateInfo.enabledLayerCount = 0;       // 已废弃
    vkDeviceCreateInfo.ppEnabledLayerNames = nullptr; // 已废弃
    vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCI.size());
    vkDeviceCreateInfo.pQueueCreateInfos = deviceQueueCI.data();

    const auto& vkDeviceFeatures = m_Features;
    VkPhysicalDeviceFeatures vkEnabledFeatures{};
    vkDeviceCreateInfo.pEnabledFeatures = &vkEnabledFeatures;

    std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    if (IsExtensionSupported(VK_KHR_MAINTENANCE1_EXTENSION_NAME))
        deviceExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME); // 支持反转viewport

    vkDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();
    vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

    VK_CHECK(vkCreateDevice(m_VkPhysicalDevice, &vkDeviceCreateInfo, nullptr, &m_VkDevice),
        "Failed to create logical device");

    if (graphicsQueueIndex >= 0)
    {
        m_GraphicQueue.QueueFamilyIndex = graphicsQueueIndex;
        vkGetDeviceQueue(m_VkDevice, graphicsQueueIndex, 0, &m_GraphicQueue.VkQueue);
    }
    else
    {
        std::runtime_error("None graphicQueue!");
    }

    if (computeQueueIndex >= 0)
    {
        m_ComputeQueue.QueueFamilyIndex = computeQueueIndex;
        vkGetDeviceQueue(m_VkDevice, graphicsQueueIndex, 0, &m_ComputeQueue.VkQueue);
    }
    else
    {
        m_ComputeQueue = m_GraphicQueue;
    }

    if (transferQueueIndex >= 0)
    {
        m_TransferQueue.QueueFamilyIndex = transferQueueIndex;
        vkGetDeviceQueue(m_VkDevice, transferQueueIndex, 0, &m_TransferQueue.VkQueue);
    }
    else
    {
        m_TransferQueue = m_GraphicQueue;
    }
}

uint32_t VulkanEngine::FindQueueFamily(VkQueueFlags QueueFlags) const
{
    uint32_t invalidFamilyInd = std::numeric_limits<uint32_t>::max();
    uint32_t familyInd = invalidFamilyInd;


    for (uint32_t i = 0; i < m_QueueFamilyProperties.size(); ++i)
    {
        const auto& Props = m_QueueFamilyProperties[i];

        if (QueueFlags == VK_QUEUE_GRAPHICS_BIT)
        {
            if ((Props.queueFlags & QueueFlags) == QueueFlags)
            {
                familyInd = i;
            }
        }
        else if (QueueFlags == VK_QUEUE_COMPUTE_BIT)
        {
            if (!(Props.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                (Props.queueFlags & VK_QUEUE_COMPUTE_BIT))
            {
                familyInd = i;
            }
        }
        else if (QueueFlags == VK_QUEUE_TRANSFER_BIT)
        {
            if (!(Props.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
                !(Props.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                (Props.queueFlags & VK_QUEUE_TRANSFER_BIT))
            {
                familyInd = i;
            }
        }

    }

    EXP_CHECK(familyInd != invalidFamilyInd, "Failed to find suitable queue family");

    return familyInd;
}

bool VulkanEngine::IsExtensionSupported(const char* ExtensionName) const
{
    for (const auto& Extension : m_SupportedExtensions)
        if (strcmp(Extension.extensionName, ExtensionName) == 0)
            return true;

    return false;
}

uint32_t VulkanEngine::GetMemoryTypeIndex(uint32_t memoryTypeBitsRequirement,
    VkMemoryPropertyFlags requiredProperties) const
{
    for (uint32_t memoryIndex = 0; memoryIndex < m_MemoryProperties.memoryTypeCount; memoryIndex++)
    {
        const uint32_t memoryTypeBit = (1 << memoryIndex);
        const bool     isRequiredMemoryType = (memoryTypeBitsRequirement & memoryTypeBit) != 0;
        if (isRequiredMemoryType)
        {
            const VkMemoryPropertyFlags properties = m_MemoryProperties.memoryTypes[memoryIndex].propertyFlags;
            const bool hasRequiredProperties = (properties & requiredProperties) == requiredProperties;

            if (hasRequiredProperties)
                return memoryIndex;
        }
    }
    return ~uint32_t{ 0 };
}