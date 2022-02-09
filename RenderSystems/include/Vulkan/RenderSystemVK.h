#pragma once

#include <memory>
#include <vector>
#include <string>
#include <vulkan.h>
#include "MathDef.h"

class VulkanSwapChain;
class VulkanInstance;
class VulkanDevice;
class VulkanBuffer;
class VulkanCommandPool;
class VulkanCommandBuffers;
class VulkanImage;
class VulkanRenderPass;

namespace Spectre
{
	class RenderSystemVK
	{
	private:
		struct GPUBuffer
		{
			VkDeviceMemory  memory;
			VkBuffer        buffer;

			GPUBuffer()
				: memory(VK_NULL_HANDLE)
				, buffer(VK_NULL_HANDLE)
			{

			}
		};

		struct UBOData
		{
			Matrix4x4 model;
			Matrix4x4 view;
			Matrix4x4 projection;
		};

	public:
		void Init();

		void Loop();

		void Exist();
	private:
		void CreateDepthStencil();
		void CreateRenderPass();
		void CreateFrameBuffer();
		void CreateSemaphores();
		void CreateFences();
		void CreateCommandBuffers();
		void CreateMeshBuffers();
		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSetLayout();
		void CreateDescriptorSet();
		void CreatePipelines();
		void SetupCommandBuffers();
		VkShaderModule LoadSPIPVShader(const std::string& filepath);
		void Draw();

		void UpdateUniformBuffers();

		void DestroyFrameBuffers();
		void DestroyDescriptorSetLayout();
		void DestroyDescriptorPool();
		void DestroyPipelines();
		void DestorySemaphores();
		void DestroyFences();
	private:
		std::shared_ptr<VulkanInstance>			m_Instance;
		std::shared_ptr<VulkanDevice>			m_Device;
		std::shared_ptr<VulkanSwapChain>		m_SwapChain;
		std::shared_ptr<VulkanCommandPool>		m_CommandPool;

		std::shared_ptr<VulkanImage>			m_DepthStencilImage;

		std::shared_ptr<VulkanRenderPass>		m_RenderPass;

		std::vector<VkFramebuffer>				m_FrameBuffers;

		VkSemaphore								m_RenderComplete = VK_NULL_HANDLE;
		std::vector<VkFence>					m_Fences;


		std::shared_ptr<VulkanCommandBuffers>	m_CommandBuffers;

		std::shared_ptr<VulkanBuffer>			m_VertexBuffer;

		uint32_t								m_IndicesCount = 0;
		std::shared_ptr<VulkanBuffer>			m_IndicesBuffer;

		std::shared_ptr<VulkanBuffer>			m_MVPBuffer;
		VkDescriptorBufferInfo					m_MVPDescriptor;
		UBOData									m_MVPData;

		VkDescriptorPool						m_DescriptorPool = VK_NULL_HANDLE;

		VkDescriptorSetLayout					m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout						m_PipelineLayout = VK_NULL_HANDLE;

		VkDescriptorSet							m_DescriptorSet = VK_NULL_HANDLE;

		VkPipeline								m_Pipeline = VK_NULL_HANDLE;
		VkPipelineCache							m_PipelineCache = VK_NULL_HANDLE;

		VkSemaphore								m_PresentComplete = VK_NULL_HANDLE;

	};

}
