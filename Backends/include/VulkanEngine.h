#pragma once
#include "VulkanCommon.h"
#include "AtExit.h"

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

	void Submit(VulkanQueue& queue, bool reset = true);

	void Reset();

	bool IsOnRecording()const { return m_OnRecording; }

public:
	std::vector<VkSemaphore> SignalSemaphore;
	std::vector < VkPipelineStageFlags> WaitStageMask;
	std::vector<VkSemaphore> WaitSemaphore;
private:
	VkDevice									 m_VkDevice;
	VkCommandPool								 m_CommandPool;
	VkCommandBuffer								 m_VkCommandBuffer;
	VkFence										 m_VkFence = VK_NULL_HANDLE;
	bool                                         m_OnRecording = false;
};


class DescriptorAllocator
{
private:
	const uint32_t MaxSets = 1000;

	const std::vector<std::pair<VkDescriptorType, float>> PoolSizes =
	{
		{ VK_DESCRIPTOR_TYPE_SAMPLER, 0.5f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2.f },
		{ VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1.f }
	};

	struct PoolUseInfo
	{
		uint32_t Index = 0;

	};

public:
	DescriptorAllocator(VkDevice device);
	~DescriptorAllocator();

	void Allocate(VkDescriptorSet* set, VkDescriptorSetLayout layout);
	void Free(VkDescriptorSet* set);
private:
	VkDescriptorPool CreatePool();

private:
	VkDevice         m_VkDevice;
	VkDescriptorPool m_CurrentPool = VK_NULL_HANDLE;
	std::vector<std::pair<VkDescriptorPool, std::vector<VkDescriptorSet>>> m_UsedPools;

};

class DescriptorLayoutCache
{
public:
	DescriptorLayoutCache(VkDevice device);
	~DescriptorLayoutCache();

	VkDescriptorSetLayout CreateDescriptorLayout(VkDescriptorSetLayoutCreateInfo* info);

	struct DescriptorLayoutInfo {

		std::vector<VkDescriptorSetLayoutBinding> Bindings;

		bool operator==(const DescriptorLayoutInfo& other) const;

		size_t Hash() const;
	};

private:

	struct DescriptorLayoutHash
	{

		std::size_t operator()(const DescriptorLayoutInfo& k) const
		{
			return k.Hash();
		}
	};
	VkDevice  m_VkDevice;
	std::unordered_map<DescriptorLayoutInfo, VkDescriptorSetLayout, DescriptorLayoutHash> m_LayoutCache;
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
	static VulkanEngine* GetInstance() 
	{
		VulkanEngine::VulkanEngineCreateInfo engineCI;
#ifdef VKB_DEBUG
		engineCI.EnableValidation = true;
#endif
		if (m_EngineInstance == nullptr)
		{
			m_EngineInstance = new VulkanEngine(engineCI);

			AtExit::AddExitFunc(0, VulkanEngine::Destory);
		}
			
		return m_EngineInstance;
	}
	static void Destory();

	VkInstance GetVkInstance()const { return m_VkInstance; }
	VkPhysicalDevice GetVkPhysicalDevice() const { return m_VkPhysicalDevice; }
	VkDevice GetVkDevice() const { return m_VkDevice; }

	VulkanQueue& GetGraphicQueue() { return m_GraphicQueue; }
	VulkanQueue& GetTransferQueue() { return m_TransferQueue; }
	VulkanQueue& GetComputeQueue() { return m_ComputeQueue; }

	uint32_t GetMemoryTypeIndex(uint32_t memoryTypeBitsRequirement,
		VkMemoryPropertyFlags requiredProperties) const;

	VkPhysicalDeviceProperties GetVkPhysicalDeviceProperties() const { return m_Properties; };
	VkPhysicalDeviceFeatures GetVkPhysicalDeviceFeatures() const { return m_Features; }
	VulkanCommandPtr GetTransformCmd() { return m_TransformCmdPtr; }
	std::vector<VulkanCommandPtr>   GetRenderCmd() { return m_RenderCmdPtrs; }
	
	DescriptorAllocator& GetDescriptorAllocator() { return *m_DescriptorAllocator; }

	DescriptorLayoutCache& GetDescriptorLayoutCache() { return *m_DescriptorLayoutCache; }

	VkPipelineCache& GetVkPipelineCache() { return m_VkPipelineCache; }
private:
	VulkanEngine(const VulkanEngineCreateInfo& CI);
	~VulkanEngine();

	void CreateVkInstance();
	void CreateVkPhysicalDevice();
	void CreateVkDevice();
	void CreateCommands();
	void CreateDescriptorAllocator();
	void CreatePipelineCache();

	uint32_t FindQueueFamily(VkQueueFlags QueueFlags) const;
	bool IsExtensionSupported(const char* ExtensionName) const;
private:
	static VulkanEngine* m_EngineInstance;

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

	std::shared_ptr<DescriptorAllocator>    m_DescriptorAllocator;
	std::shared_ptr<DescriptorLayoutCache>	m_DescriptorLayoutCache;

	VkPipelineCache							m_VkPipelineCache = VK_NULL_HANDLE;
};


END_NAMESPACE_SPECTRE
