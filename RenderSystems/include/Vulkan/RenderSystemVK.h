#pragma once

#include <memory>
#include <vector>
#include <vulkan.h>

class VulkanSwapChain;
class VulkanInstance;
class VulkanDevice;

namespace Spectre
{
	class RenderSystemVK
	{
	public:
		void Init();
	private:
		void CreateDepthStencil();
		void CreateRenderPass();
		void CreateFrameBuffer();
		//void CreateSemaphores();
		//void CreateFences();
		//void CreateVerticesBuffer();
		//void CreateIndexBuffer();
		//void CreateUniformBuffer();
		//void CreateDescriptorPool();
		//void CreateDescriptorSetLayout();
	private:
		std::shared_ptr<VulkanInstance>     m_Instance;
		std::shared_ptr<VulkanDevice>       m_Device;
		std::shared_ptr<VulkanSwapChain>	m_SwapChain;

		VkImage                         m_DepthStencilImage = VK_NULL_HANDLE;
		VkImageView                     m_DepthStencilView = VK_NULL_HANDLE;
		VkDeviceMemory                  m_DepthStencilMemory = VK_NULL_HANDLE;

		VkRenderPass                    m_RenderPass = VK_NULL_HANDLE;

		std::vector<VkFramebuffer>      m_FrameBuffers;
	};

}
