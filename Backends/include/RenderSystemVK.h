#pragma once
#include "SpectreApi.h"
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "NativeWindow.h"
#include "RenderTypes.h"
#include "VulkanPipeline.h"
#include "VulkanCommand.h"
#include "Math/MathDef.h"
namespace Spectre
{
	struct RenderContextDesc
	{
		bool Headless = false;
		NativeWindow Window;
	};
	class VulkanEngine;
	class VulkanContext;
	class VulkanSwapChain;
	class VulkanBuffer;
	class VulkanPrimitive;
	class VulkanImages;
	class VulkanRenderPass;
	class VulkanFrameBuffer;
	class VulkanSemaphore;
	class VulkanPipeline;
	class VulkanGui;

	class RenderSystemVK
	{
	public:
		explicit RenderSystemVK() noexcept;
		~RenderSystemVK();
		void CreateRenderContext(const RenderContextDesc& desc);

		void CreateSwapChain(const SwapChainDesc& desc);

		//VulkanPrimitive* AddPrimitive(std::shared_ptr<VulkanPipeline> pPipeline,
		//	Vertex* vertices,uint32_t vertCount, uint32_t* indices,uint32_t indCount);

		void CreatePipeline(VulkanPipelinePtr pipeline);

		//void UpdateUniformBuffers(void* pBuffe);

		void RecordCmd(std::function<void(VkCommandBuffer)> cmd);

		void Draw();

		void ReceateSwapchain( const SwapChainDesc& desc);
	private:
		void CreateDepthStencil();
		void CreateRenderPass();
		void CreateFrameBuffer();

		void DestorySwapchain();

		bool UpdateUI();
	private:
		uint32_t                                m_Width = 0;
		uint32_t                                m_Height = 0;
		VulkanEngine*							m_VulkanEnginePtr;
		std::shared_ptr<VulkanContext>			m_ContextPtr;
		std::shared_ptr<VulkanSwapChain>		m_SwapChain;

		std::shared_ptr<VulkanImages>			m_DepthStencilImage;

		std::shared_ptr<VulkanRenderPass>		m_RenderPass;

		std::vector<std::shared_ptr<VulkanFrameBuffer>>	m_FrameBuffers;

		std::shared_ptr<VulkanSemaphore>		m_RenderComplete;

		std::unordered_map<SpectreId, VulkanPrimitive*>   m_Primitives;

		//std::vector<VulkanCommandPtr>			m_RenderCommands;

		std::shared_ptr<VulkanSemaphore>		m_PresentComplete;

		std::shared_ptr<VulkanGui>              m_GuiPtr;
	};

}
