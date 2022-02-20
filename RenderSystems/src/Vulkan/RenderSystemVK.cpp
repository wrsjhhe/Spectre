#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSurface.h"
#include "VulkanSwapchain.h"
#include "VulkanCommandPool.h"
#include "VulkanCommandBuffer.h"
#include "VulkanBuffer.h"
#include "VulkanImages.h"
#include "VulkanRenderPass.h"
#include "VulkanFrameBuffer.h"
#include "VulkanSemaphore.h"
#include "VulkanFence.h"
#include "VulkanDescriptorPool.h"
#include "VulkanDescriptorSetLayout.h"
#include "VulkanDescriptorSet.h"
#include "VulkanPipeline.h"
#include "RenderSystemVK.h"

#include "Geometry/Vertex.h"


USING_NAMESPACE(Spectre)
struct UBOData
{
	Matrix4x4 model;
	Matrix4x4 view;
	Matrix4x4 projection;
};
void RenderSystemVK::CreateRenderContext()
{
	VulkanInstance::CreateInfo instanceCI;
#ifdef VKB_DEBUG
	instanceCI.EnableValidation = true;
#endif

	m_Instance = VulkanInstance::Create(instanceCI);
	VkPhysicalDevice vkPhysicalDevice = m_Instance->GetVkPhysicalDevices().at(0);

	std::shared_ptr<VulkanPhysicalDevice> physicalDevice = VulkanPhysicalDevice::Create(vkPhysicalDevice, *m_Instance->GetSharedPtr());

	m_Device = VulkanDevice::Create(physicalDevice);

	CreateCommandPool();
	CreateSemaphores();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	CreateDescriptorSet();


}

void Spectre::RenderSystemVK::CreateSurface(const NativeWindow& wnd)
{
	m_Surface = VulkanSurface::CreateSurface(*m_Instance, wnd);
}

void Spectre::RenderSystemVK::CreateSwapChain(const SwapChainDesc& desc)
{
	m_Width = desc.Width;
	m_Height = desc.Height;
	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Instance, *m_Device,*m_Surface, desc);

	CreateDepthStencil();
	CreateRenderPass();
	CreateFrameBuffer();
	CreateFences();
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

	m_RenderCommandBuffers = VulkanCommandBuffer::CreataGraphicBuffers(*m_Device, *m_CommandPool, m_SwapChain->GetImageCount());
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
			vkCmdBindVertexBuffers(cmdBuffer, 0, 1, &m_VertexBuffer->GetVkBuffer(), offsets);
			vkCmdBindIndexBuffer(cmdBuffer, m_IndicesBuffer->GetVkBuffer(), 0, VK_INDEX_TYPE_UINT32);
			vkCmdDrawIndexed(cmdBuffer, m_IndicesCount, 1, 0, 0, 0);
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
	m_RenderPass = VulkanRenderPass::CreateCommonRenderPass(*m_Device, m_SwapChain->GetSwapChainFormat());
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

void Spectre::RenderSystemVK::CreateFences()
{
	uint32_t frameCount = m_SwapChain->GetImageCount();


	m_Fences.resize(frameCount);
	for (uint32_t i = 0; i < m_Fences.size(); ++i)
	{
		m_Fences[i] = VulkanFence::CreateFence(*m_Device);
	}
}

void Spectre::RenderSystemVK::CreateCommandPool()
{
	m_CommandPool = VulkanCommandPool::CreateCommandPool(*m_Device);
}


void Spectre::RenderSystemVK::CreateMeshBuffers(const std::vector<Vertex> vertices,const std::vector<uint32_t> indices)
{
	VkDevice device = m_Device->GetVkDevice();
	VulkanQueue queue = m_Device->GetGraphicQueue();


	m_IndicesCount = (uint32_t)indices.size();

	uint32_t vertexBufferSize = vertices.size() * sizeof(Vertex);
	uint32_t indexBufferSize = (uint32_t)indices.size() * sizeof(uint32_t);


	auto tempVertexBuffer = VulkanBuffer::CreateHostBuffer(*m_Device,vertices.data(), vertices.size() * sizeof(Vertex));
	auto tempIndexBuffer = VulkanBuffer::CreateHostBuffer(*m_Device, indices.data(), m_IndicesCount * sizeof(uint32_t));

	m_VertexBuffer = VulkanBuffer::CreateDeviceVertexBuffer(*m_Device, vertexBufferSize);
	m_IndicesBuffer = VulkanBuffer::CreateDeviceIndexBuffer(*m_Device, indexBufferSize);

	auto xferCmdBuffer = VulkanCommandBuffer::CreataGraphicBuffers(*m_Device, *m_CommandPool, 1)[0];
	//VkCommandBuffer vkCmdBuffer = xferCmdBuffer->GetVkCommandBuffer();

	// 开始录制命令
	//VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	//cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	//vkBeginCommandBuffer(vkCmdBuffer, &cmdBufferBeginInfo);

	tempVertexBuffer->MapToDevice(*m_VertexBuffer, xferCmdBuffer->GetVkCommandBuffer());
	tempIndexBuffer->MapToDevice(*m_IndicesBuffer, xferCmdBuffer->GetVkCommandBuffer());

	xferCmdBuffer->Submit(queue);
	// 结束录制
	//vkEndCommandBuffer(vkCmdBuffer);

	// 提交命令，并且等待命令执行完毕。
	//VkSubmitInfo submitInfo{};
	//submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	//submitInfo.commandBufferCount = 1;
	//submitInfo.pCommandBuffers = &vkCmdBuffer;

	//VkFenceCreateInfo fenceInfo{};
	//fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//fenceInfo.flags = 0;

	//VkFence fence = VK_NULL_HANDLE;
	//vkCreateFence(device, &fenceInfo, nullptr, &fence);
	//vkQueueSubmit(queue.GetVkQueue(), 1, &submitInfo, fence);
	//vkWaitForFences(device, 1, &fence, VK_TRUE, MAX_int64);

	//vkDestroyFence(device, fence, nullptr);
}


void Spectre::RenderSystemVK::CreateUniformBuffers()
{
	m_MVPBuffer = VulkanBuffer::CreateHostUniformBuffer(*m_Device,nullptr, sizeof(UBOData));

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
	m_MVPData.projection.SetIdentity();
	m_MVPData.projection.Perspective(DegreesToRadians(75.0f), m_Width, m_Height, 0.01f, 3000.0f);
	VkDevice device = m_Device->GetVkDevice();
	//m_MVPData.model.AppendRotation(90.0f * delta, Vector3::UpVector);
	m_MVPBuffer->UpdateHostBuffer(&m_MVPData);
}

void Spectre::RenderSystemVK::ReceateSwapchain(const SwapChainDesc& desc)
{
	vkDeviceWaitIdle(m_Device->GetVkDevice());

	DestorySwapchain();
	m_Width = desc.Width;
	m_Height = desc.Height;
	
	m_SwapChain = std::make_shared<VulkanSwapChain>(*m_Instance, *m_Device,*m_Surface, desc);
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
