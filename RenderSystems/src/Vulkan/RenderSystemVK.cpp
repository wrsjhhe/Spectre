#include "VulkanCommon.h"
#include "VulkanContext.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"
#include "VulkanCommand.h"
#include "VulkanBuffer.h"
#include "VulkanImages.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"
#include "VulkanIndexBuffer.h"
#include "VulkanVertexBuffer.h"
#include "RenderSystemVK.h"

#include "Geometry/Vertex.h"


USING_NAMESPACE(Spectre)

Spectre::RenderSystemVK::RenderSystemVK() noexcept
{
	VulkanInstance::CreateInfo instanceCI;
#ifdef VKB_DEBUG
	instanceCI.EnableValidation = true;
#endif

	m_Instance = VulkanInstance::Create(instanceCI);
	VkPhysicalDevice vkPhysicalDevice = m_Instance->GetVkPhysicalDevices().at(0);

	m_PhysicalDevice = VulkanPhysicalDevice::Create(vkPhysicalDevice, *m_Instance->GetSharedPtr());

	m_Device = VulkanDevice::Create(m_PhysicalDevice);

	m_ContextPtr = std::make_shared<VulkanContext>(m_Instance->GetVkInstance(), m_PhysicalDevice->GetVkPhysicalDevice(), *m_Device);
}

Spectre::RenderSystemVK::~RenderSystemVK()
{
	
}

void RenderSystemVK::CreateRenderContext(const NativeWindow& wnd)
{
	//创建Surface
	if (m_ContextPtr->m_VkSurface != VK_NULL_HANDLE)
	{
		vkDestroySurfaceKHR(m_Instance->GetVkInstance(), m_ContextPtr->m_VkSurface, NULL);
		m_ContextPtr->m_VkSurface = VK_NULL_HANDLE;
	}

	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo{};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hinstance = GetModuleHandle(NULL);
	surfaceCreateInfo.hwnd = (HWND)wnd.hWnd;
	VkResult err = vkCreateWin32SurfaceKHR(m_Instance->GetVkInstance(), &surfaceCreateInfo, nullptr, &m_ContextPtr->m_VkSurface);

	VK_CHECK(err, "Failed create surface!");

	m_ContextPtr->Init();

	CreateSemaphores();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	CreateDescriptorSet();
}


void Spectre::RenderSystemVK::CreateSwapChain(const SwapChainDesc& desc)
{
	m_Width = desc.Width;
	m_Height = desc.Height;
	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Device,*m_ContextPtr, desc);

	CreateDepthStencil();
	CreateRenderPass();
	CreateFrameBuffer();
	CreatePipelines();
}

void Spectre::RenderSystemVK::Setup()
{
	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkClearValue clearValues[2];
	clearValues[0].color = {
		{0.350f, 0.350f, 0.350f, 1.0f}
	};
	clearValues[1].depthStencil = { 1.0f, 0 };

	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = m_RenderPass->GetVkRenderPass();
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = fwidth;
	renderPassBeginInfo.renderArea.extent.height = fheight;

	m_RenderCommandBuffers = VulkanCommand::Create(*m_Device, m_ContextPtr->GetVkGraphicCommandPool(), m_SwapChain->GetImageCount());
	for (uint32_t i = 0; i < m_RenderCommandBuffers.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = m_FrameBuffers[i]->GetVkFrameBuffer();

		VkViewport viewport = {};
		viewport.x = 0;
		viewport.y = (float)fheight;
		viewport.width = (float)fwidth;
		viewport.height = -(float)fheight;    // flip y axis
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor = {};
		scissor.extent.width = fwidth;
		scissor.extent.height = fheight;
		scissor.offset.x = 0;
		scissor.offset.y = 0;

		VkDeviceSize offsets[1] = { 0 };

		m_RenderCommandBuffers[i]->RecordCommond([&](VkCommandBuffer cmdBuffer) {
			vkCmdBeginRenderPass(cmdBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
			vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);
			vkCmdSetScissor(cmdBuffer, 0, 1, &scissor);
			vkCmdBindDescriptorSets(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetVkPipelineLayout(), 0, 1, &m_DescriptorSet->GetVkDescriptorSet(), 0, nullptr);
			vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetVkPipeline());
			m_VertexBuffer->CmdBind(cmdBuffer, offsets);
			m_IndicesBuffer->CmdBind(cmdBuffer);
			vkCmdDrawIndexed(cmdBuffer, m_IndicesBuffer->GetIndexCount(), 1, 0, 0, 0);
			vkCmdEndRenderPass(cmdBuffer);		
		});

	}
	vkDeviceWaitIdle(m_Device->GetVkDevice());
}

void Spectre::RenderSystemVK::Draw()
{
	UpdateUniformBuffers();

	VkDevice device = m_Device->GetVkDevice();
	VulkanQueue queue = m_Device->GetGraphicQueue();
	int32_t backBufferIndex = m_SwapChain->AcquireImageIndex(m_PresentComplete);
	if (backBufferIndex < 0)
	{
		ReceateSwapchain({ m_Width,m_Height });
		return;
	}

	// 提交绘制命令
	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	m_RenderCommandBuffers[backBufferIndex]->SignalSemaphore = { m_RenderComplete->GetVkSemaphore() };
	m_RenderCommandBuffers[backBufferIndex]->WaitSemaphore = { m_PresentComplete->GetVkSemaphore() };
	m_RenderCommandBuffers[backBufferIndex]->WaitStageMask = { waitStageMask };

	m_RenderCommandBuffers[backBufferIndex]->Submit(queue);

	// present
	m_SwapChain->Present(
		queue.GetVkQueue(),
		&m_RenderComplete->GetVkSemaphore()
	);
}

void Spectre::RenderSystemVK::CreateDepthStencil()
{
	m_DepthStencilImage = VulkanImages::CreateDepthStencilImage(*m_Device, m_Width, m_Height);
}

void Spectre::RenderSystemVK::CreateRenderPass()
{
	m_RenderPass = VulkanRenderPass::CreateCommonRenderPass(*m_Device, m_ContextPtr->m_VkSwapChainFormat);
}

void Spectre::RenderSystemVK::CreateFrameBuffer()
{
	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();
	VkDevice device = m_Device->GetVkDevice();

	std::vector<VkImageView> attachments;
	attachments.resize(2);
	attachments[1] = m_DepthStencilImage->GetVkImageViews()[0];

	const std::vector<VkImageView>& backbufferViews = m_SwapChain->GetImages().GetVkImageViews();

	m_FrameBuffers.resize(backbufferViews.size());
	for (uint32_t i = 0; i < m_FrameBuffers.size(); ++i)
	{
		attachments[0] = backbufferViews[i];
		m_FrameBuffers[i] = VulkanFrameBuffer::CreateFrameBuffer(*m_Device, *m_RenderPass, attachments, m_Width, m_Height);
	}
}

void Spectre::RenderSystemVK::CreateSemaphores()
{
	m_RenderComplete = VulkanSemaphore::CreateSemaphore(*m_Device);
}



void Spectre::RenderSystemVK::CreateMeshBuffers(std::vector<float>& vertices,std::vector<uint32_t>& indices)
{
	VkDevice device = m_Device->GetVkDevice();
	VulkanQueue queue = m_Device->GetGraphicQueue();

	m_IndicesBuffer = VulkanIndexBuffer::Create(*m_Device, indices, VK_INDEX_TYPE_UINT32);

	std::vector<VertexAttribute> attributes{ VertexAttribute_Position ,VertexAttribute_Color };
	m_VertexBuffer = VulkanVertexBuffer::Create(*m_Device, vertices, { VertexAttribute_Position ,VertexAttribute_Color });

	auto xferCmdBuffer = VulkanCommand::Create(*m_Device, m_ContextPtr->GetVkGraphicCommandPool(), 1)[0];

	xferCmdBuffer->RecordCommond([&](VkCommandBuffer cmdBuffer) {
		m_VertexBuffer->Synchronize(cmdBuffer);
		m_IndicesBuffer->Synchronize(cmdBuffer);
	});

	xferCmdBuffer->Submit(queue);
}


void Spectre::RenderSystemVK::CreateUniformBuffers()
{
	m_MVPBuffer = VulkanBuffer::Create(*m_Device, sizeof(UBOData), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, 
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	m_MVPData.model.SetIdentity();
	m_MVPData.model.SetOrigin(0, 0, 0);

	m_MVPData.view.SetIdentity();
	m_MVPData.view.m[3][2] = 2.5f; 

	m_MVPData.projection.SetIdentity();
	m_MVPData.projection.Perspective(DegreesToRadians(75.0f), m_Width, m_Height, 0.01f, 3000.0f);
}

void Spectre::RenderSystemVK::CreateDescriptorPool()
{
	m_DescriptorPool = VulkanDescriptorPool::CreateDescriptorPool(*m_Device);
}

void Spectre::RenderSystemVK::CreateDescriptorSetLayout()
{
	m_DescriptorSetLayout = VulkanDescriptorSetLayout::CreateDescriptorSetLayout(*m_Device);
}

void Spectre::RenderSystemVK::CreateDescriptorSet()
{
	VkDevice device = m_Device->GetVkDevice();

	m_DescriptorSet = VulkanDescriptorSet::CreateDescriptorSet(*m_Device, *m_DescriptorPool, *m_DescriptorSetLayout);
	VkDescriptorBufferInfo MVPDescriptor;
	MVPDescriptor.buffer = m_MVPBuffer->GetVkBuffer();
	MVPDescriptor.offset = 0;
	MVPDescriptor.range = sizeof(UBOData);

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_DescriptorSet->GetVkDescriptorSet();
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &MVPDescriptor;
	writeDescriptorSet.dstBinding = 0;
	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
}

void Spectre::RenderSystemVK::CreatePipelines()
{
	m_Pipeline = VulkanPipeline::CreatePipeline(*m_Device, *m_RenderPass, *m_DescriptorSetLayout);
}




void Spectre::RenderSystemVK::UpdateUniformBuffers()
{
	static float  delta = 0;
	delta += 0.00001;
	if (delta > 6.28)
		delta = 0;
	m_MVPData.projection.SetIdentity();
	m_MVPData.projection.Perspective(DegreesToRadians(75.0f), m_Width, m_Height, 0.01f, 3000.0f);
	VkDevice device = m_Device->GetVkDevice();
	m_MVPData.model.AppendRotation(90.0f * delta, Vector3(0.f,0.f,1.f));
	m_MVPBuffer->UpdateHostBuffer(&m_MVPData);
}

void Spectre::RenderSystemVK::ReceateSwapchain(const SwapChainDesc& desc)
{
	SwapChainDesc _desc = desc;
	vkDeviceWaitIdle(m_Device->GetVkDevice());

	DestorySwapchain();
	m_ContextPtr->InitSwapchainParamaters();
	m_ContextPtr->CalculateSwapChainExtent(_desc.Width, _desc.Height);
	m_Width = _desc.Width;
	m_Height = _desc.Height;

	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Device,*m_ContextPtr, _desc);
	CreateDepthStencil();
	CreateSemaphores();
	CreateRenderPass();
	CreateFrameBuffer();
	Setup();
}

void Spectre::RenderSystemVK::DestorySwapchain()
{
	m_FrameBuffers.clear();

	m_DepthStencilImage = nullptr;

	m_RenderCommandBuffers.clear();

	m_RenderPass = nullptr;

	m_PresentComplete = nullptr;

	m_SwapChain = nullptr;
}
