#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include "Math/MathDef.h"
#include "NativeWindow.h"
#include "RenderDef.h"

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
	class VulkanIndexBuffer;
	class VulkanVertexBuffer;
	class VulkanCommand;
	class VulkanImages;
	class VulkanRenderPass;
	class VulkanFrameBuffer;
	class VulkanSemaphore;
	class VulkanPipelineCache;
	class VulkanGui;
	typedef std::shared_ptr<VulkanCommand> VulkanCommandPtr;
	typedef std::shared_ptr<VulkanBuffer> VulkanBufferPtr;

	struct VulkanPrimitive
	{
		std::shared_ptr<VulkanVertexBuffer>		VertexBufferPtr;

		std::shared_ptr <VulkanIndexBuffer>		IndicesBufferPtr;
	};

	class RenderSystemVK
	{
	public:
		explicit RenderSystemVK() noexcept;
		~RenderSystemVK();
		void CreateRenderContext(const RenderContextDesc& desc);

		void CreateSwapChain(const SwapChainDesc& desc);

		void AddMeshBuffer(std::vector<float>& vertices,std::vector<uint32_t>& indices);

		void CreatePipeline(const PipelineDesc& pipelineDesc);

		void UpdateUniformBuffers(const Matrix& mat);

		void Setup();

		void Draw();

		void ReceateSwapchain( const SwapChainDesc& desc);
	private:
		void CreateDepthStencil();
		void CreateRenderPass();
		void CreateFrameBuffer();
		void CreateSemaphores();

		void DestorySwapchain();

		bool UpdateUI();
	private:
		uint32_t                                m_Width = 1400;
		uint32_t                                m_Height = 900;
		VulkanEngine*							m_VulkanEnginePtr;
		std::shared_ptr<VulkanContext>			m_ContextPtr;
		std::shared_ptr<VulkanSwapChain>		m_SwapChain;

		std::shared_ptr<VulkanImages>			m_DepthStencilImage;

		std::shared_ptr<VulkanRenderPass>		m_RenderPass;

		std::vector<std::shared_ptr<VulkanFrameBuffer>>	m_FrameBuffers;

		std::shared_ptr<VulkanSemaphore>		m_RenderComplete;

		std::unordered_map<void*, VulkanPrimitive*>   m_Primitives;

		VulkanBufferPtr							m_MVPBuffer;

		std::vector<VulkanCommandPtr>			m_RenderCommandBuffers;

		std::vector<VulkanPipelineCache*>       m_PipelineCachePtrs;

		std::shared_ptr<VulkanSemaphore>		m_PresentComplete;

		std::shared_ptr<VulkanGui>              m_GuiPtr;
	};

}
