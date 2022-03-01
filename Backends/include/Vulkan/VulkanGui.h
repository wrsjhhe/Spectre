#pragma once
#include <string>
#include "Math/MathDef.h"
#include "VulkanEngine.h"

BEGIN_NAMESPACE_SPECTRE

class VulkanGui
{
public:
	VulkanGui(VulkanEngine* pEngine);

	virtual ~VulkanGui();

protected:

	struct UIBuffer
	{
		VkBuffer        buffer;
		VkDeviceMemory  memory;
		VkDevice        device;
		void* mapped;
		VkDeviceSize    size;
		VkDeviceSize    alignment;

		UIBuffer()
			: buffer(VK_NULL_HANDLE)
			, memory(VK_NULL_HANDLE)
			, device(VK_NULL_HANDLE)
			, mapped(nullptr)
			, size(0)
			, alignment(0)
		{

		}

		void Unmap()
		{
			if (mapped)
			{
				vkUnmapMemory(device, memory);
				mapped = nullptr;
			}
		}

		VkResult Map(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			return vkMapMemory(device, memory, offset, size, 0, &mapped);
		}

		void CopyFrom(void* data, VkDeviceSize size)
		{
			memcpy(mapped, data, size);
		}

		VkResult Flush(VkDeviceSize size = VK_WHOLE_SIZE, VkDeviceSize offset = 0)
		{
			VkMappedMemoryRange mappedRange = {};
			mappedRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
			mappedRange.memory = memory;
			mappedRange.offset = offset;
			mappedRange.size = size;
			return vkFlushMappedMemoryRanges(device, 1, &mappedRange);
		}

		void Destroy()
		{
			if (buffer)
			{
				vkDestroyBuffer(device, buffer, nullptr);
				buffer = VK_NULL_HANDLE;
			}

			if (memory)
			{
				vkFreeMemory(device, memory, nullptr);
				memory = VK_NULL_HANDLE;
			}

			device = VK_NULL_HANDLE;
		}
	};

	struct PushConstBlock
	{
		Vector2 scale;
		Vector2 translate;
	};
public:

	void Init(const std::string& font, uint32_t width, uint32_t height);

	void Destroy();

	void Resize(uint32_t width, uint32_t height);

	bool Update();

	void StartFrame();

	void EndFrame();

	void BindDrawCmd(const VkCommandBuffer& commandBuffer, const VkRenderPass& renderPass, int32_t subpass = 0, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	__inline float GetScale() const
	{
		return m_Scale;
	}

protected:

	void PrepareFontResources();

	void PreparePipelineResources();

	void PreparePipeline(VkRenderPass renderPass, int32_t subpass, VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT);

	void CreateBuffer(UIBuffer& buffer, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, VkDeviceSize size);

protected:

	VulkanEngine*			m_VulkanEnginePtr;

	UIBuffer                m_VertexBuffer;
	UIBuffer                m_IndexBuffer;
	int32_t                 m_VertexCount;
	int32_t                 m_IndexCount;

	int32_t                 m_Subpass;

	VkDescriptorPool        m_DescriptorPool;
	VkDescriptorSetLayout   m_DescriptorSetLayout;
	VkDescriptorSet         m_DescriptorSet;
	VkPipelineLayout        m_PipelineLayout;

	VkPipelineCache         m_PipelineCache;
	VkPipeline              m_Pipeline;

	VkRenderPass            m_LastRenderPass;
	int32_t                 m_LastSubPass = -1;
	VkSampleCountFlagBits   m_LastSampleCount = VK_SAMPLE_COUNT_1_BIT;

	VkDeviceMemory          m_FontMemory;
	VkImage                 m_FontImage;
	VkImageView             m_FontView;
	VkSampler               m_FontSampler;

	PushConstBlock          m_PushData;

	bool                    m_Visible;
	bool                    m_Updated;
	float                   m_Scale;

	std::string             m_FontPath;
};

END_NAMESPACE_SPECTRE