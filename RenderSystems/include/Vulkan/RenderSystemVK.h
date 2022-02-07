#pragma once

#include <memory>
#include <vector>
#include <string>
#include <vulkan.h>
#include "MathDef.h"

class VulkanSwapChain;
class VulkanInstance;
class VulkanDevice;

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
	private:
		std::shared_ptr<VulkanInstance>     m_Instance;
		std::shared_ptr<VulkanDevice>       m_Device;
		std::shared_ptr<VulkanSwapChain>	m_SwapChain;

		VkImage                         m_DepthStencilImage = VK_NULL_HANDLE;
		VkImageView                     m_DepthStencilView = VK_NULL_HANDLE;
		VkDeviceMemory                  m_DepthStencilMemory = VK_NULL_HANDLE;

		VkRenderPass                    m_RenderPass = VK_NULL_HANDLE;

		std::vector<VkFramebuffer>      m_FrameBuffers;

		VkSemaphore                     m_RenderComplete = VK_NULL_HANDLE;
		std::vector<VkFence>            m_Fences;

		VkCommandPool                   m_CommandPool = VK_NULL_HANDLE;
		std::vector<VkCommandBuffer>    m_CommandBuffers;

		GPUBuffer						m_VertexBuffer;

		uint32_t                        m_IndicesCount = 0;
		GPUBuffer						m_IndicesBuffer;

		GPUBuffer                       m_MVPBuffer;
		VkDescriptorBufferInfo          m_MVPDescriptor;
		UBOData                         m_MVPData;

		VkDescriptorPool                m_DescriptorPool = VK_NULL_HANDLE;

		VkDescriptorSetLayout           m_DescriptorSetLayout = VK_NULL_HANDLE;
		VkPipelineLayout                m_PipelineLayout = VK_NULL_HANDLE;

		VkDescriptorSet                 m_DescriptorSet = VK_NULL_HANDLE;

		VkPipeline                      m_Pipeline = VK_NULL_HANDLE;
		VkPipelineCache                 m_PipelineCache = VK_NULL_HANDLE;

		VkSemaphore                     m_PresentComplete = VK_NULL_HANDLE;

	};

}
