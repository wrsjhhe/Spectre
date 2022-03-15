#pragma once

BEGIN_NAMESPACE_SPECTRE


struct VulkanQueue
{
	uint32_t   QueueFamilyIndex = -1;
	VkQueue    VkQueue = VK_NULL_HANDLE;
};

class VulkanCommand;
typedef std::shared_ptr<VulkanCommand> VulkanCommandPtr;
class VulkanCommand
{
public:
	VulkanCommand(VkDevice device,const VkCommandPool& commandPool);

	~VulkanCommand();
	VkCommandBuffer& GetVkCommandBuffer() { return m_VkCommandBuffer; }

	void RecordCommond(std::function<void(VkCommandBuffer)> recordCmd);

	void Submit(VulkanQueue& queue);

	void Reset();

public:
	std::vector<VkSemaphore> SignalSemaphore;
	std::vector < VkPipelineStageFlags> WaitStageMask;
	std::vector<VkSemaphore> WaitSemaphore;
private:
	VkDevice									 m_VkDevice;
	VkCommandPool								 m_CommandPool;
	VkCommandBuffer								 m_VkCommandBuffer;
	VkFence										 m_VkFence = VK_NULL_HANDLE;
};

class VulkanEngine
{
public:
	struct VulkanEngineCreateInfo
	{
		bool                   EnableValidation = false;
		bool                   EnableDeviceSimulation = false;
		uint32_t               InstanceExtensionCount = 0;
		const char* const*	   ppInstanceExtensionNames = nullptr;
	};
public:
	static VulkanEngine* m_EngineInstance;
	static VulkanEngine* Create(const VulkanEngineCreateInfo& CI);
	static VulkanEngine* GetInstance() 
	{ 
		return m_EngineInstance; 
	}
	static void Destory();

	VkInstance GetVkInstance()const { return m_VkInstance; }
	VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
	VkDevice GetVkDevice() const { return m_VkDevice; }

	const VulkanQueue& GetGraphicQueue() const { return m_GraphicQueue; }
	const VulkanQueue& GetTransferQueue() const { return m_TransferQueue; }
	const VulkanQueue& GetComputeQueue() const { return m_ComputeQueue; }

	uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBitsRequirement,
		VkMemoryPropertyFlags requiredProperties) const;

	VkPhysicalDeviceLimits GetVkPhysicalDeviceLimits();

	VulkanCommandPtr GetTransformCmd() { return m_TransformCmdPtr; }
	std::vector<VulkanCommandPtr>   GetRenderCmd() { return m_RenderCmdPtrs; }

private:
	VulkanEngine(const VulkanEngineCreateInfo& CI);
	~VulkanEngine();

	void CreateVkInstance();
	void CreateVkPhysicalDevice();
	void CreateVkDevice();
	void CreateCommands();

	uint32_t FindQueueFamily(VkQueueFlags QueueFlags) const;
	bool IsExtensionSupported(const char* ExtensionName) const;
private:
	VulkanEngineCreateInfo					m_CI;
	bool									m_DebugUtilsEnabled = false;
	VkInstance								m_VkInstance = VK_NULL_HANDLE;
	std::vector<const char*>				m_EnabledExtensions;
	VkDebugUtilsMessengerEXT				debug_utils_messenger{ VK_NULL_HANDLE };

	VkPhysicalDevice						m_VkPhysicalDevice;
	VkPhysicalDeviceProperties				m_Properties = {};
	VkPhysicalDeviceFeatures				m_Features = {};
	VkPhysicalDeviceMemoryProperties		m_MemoryProperties = {};
	std::vector<VkQueueFamilyProperties>	m_QueueFamilyProperties;
	std::vector<VkExtensionProperties>		m_SupportedExtensions;

	VkDevice								m_VkDevice = VK_NULL_HANDLE;
	VulkanQueue								m_GraphicQueue;
	VulkanQueue								m_ComputeQueue;
	VulkanQueue								m_TransferQueue;

	VkCommandPool                           m_VkCommandPool;
	VulkanCommandPtr                        m_TransformCmdPtr;
	std::vector<VulkanCommandPtr>           m_RenderCmdPtrs;

};


END_NAMESPACE_SPECTRE
