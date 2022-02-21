#pragma once

#include <memory>
#include <vector>
#include <string>
#include "MathDef.h"
#include "NativeWindow.h"
#include "VulkanGraphicTypes.h"
#include "Geometry/Vertex.h"

namespace Spectre
{
	class VulkanSurface;
	class VulkanSwapChain;
	class VulkanInstance;
	class VulkanDevice;
	class VulkanBuffer;
	class VulkanCommandPool;
	class VulkanCommandBuffer;
	class VulkanImages;
	class VulkanRenderPass;
	class VulkanFrameBuffer;
	class VulkanSemaphore;
	class VulkanFence;
	class VulkanDescriptorPool;
	class VulkanDescriptorSetLayout;
	class VulkanDescriptorSet;
	class VulkanPipeline;

	typedef std::shared_ptr<VulkanCommandBuffer> VulkanCommandBufferPtr;

	class RenderSystemVK
	{
	private:
		struct UBOData
		{
			Matrix4x4 model;
			Matrix4x4 view;
			Matrix4x4 projection;
		};

	public:
		void CreateRenderContext();

		void CreateSurface(const NativeWindow& wnd);

		void CreateSwapChain(const SwapChainDesc& desc);

		void CreateMeshBuffers(const std::vector<Vertex> vertices,const std::vector<uint32_t> indices);

		void Setup();

		void Draw();

		void ReceateSwapchain( const SwapChainDesc& desc);
	private:
		void CreateDepthStencil();
		void CreateRenderPass();
		void CreateFrameBuffer();
		void CreateSemaphores();
		void CreateFences();
		void CreateCommandPool();

		void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSetLayout();
		void CreateDescriptorSet();
		void CreatePipelines();


		void UpdateUniformBuffers();

		void DestorySwapchain();
	private:
		uint32_t                                m_Width = 1400;
		uint32_t                                m_Height = 900;
		std::shared_ptr<VulkanInstance>			m_Instance;
		std::shared_ptr<VulkanDevice>			m_Device;
		std::shared_ptr<VulkanSurface>          m_Surface;
		std::shared_ptr<VulkanSwapChain>		m_SwapChain;
		std::shared_ptr<VulkanCommandPool>		m_CommandPool;

		std::shared_ptr<VulkanImages>			m_DepthStencilImage;

		std::shared_ptr<VulkanRenderPass>		m_RenderPass;

		std::vector<std::shared_ptr<VulkanFrameBuffer>>	m_FrameBuffers;

		std::shared_ptr<VulkanSemaphore>		m_RenderComplete;

		std::shared_ptr<VulkanBuffer>			m_VertexBuffer;

		uint32_t								m_IndicesCount = 0;
		std::shared_ptr<VulkanBuffer>			m_IndicesBuffer;

		std::shared_ptr<VulkanBuffer>			m_MVPBuffer;
		UBOData									m_MVPData;

		std::vector<VulkanCommandBufferPtr>		m_RenderCommandBuffers;

		std::shared_ptr<VulkanDescriptorPool>	m_DescriptorPool;

		std::shared_ptr<VulkanDescriptorSetLayout>	m_DescriptorSetLayout;

		std::shared_ptr<VulkanDescriptorSet>	m_DescriptorSet;
		std::shared_ptr<VulkanPipeline>         m_Pipeline;

		std::shared_ptr<VulkanSemaphore>		m_PresentComplete;

	};

}
