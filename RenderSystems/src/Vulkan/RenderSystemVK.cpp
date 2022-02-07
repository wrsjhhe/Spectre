#include "VulkanCommon.h"
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanDevice.h"
#include "VulkanSwapchain.h"

#include "RenderSystemVK.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <glfw3.h>
#include <glfw3native.h>

USING_NAMESPACE(Spectre)
struct Vertex
{
	float position[3];
	float color[3];
};

struct UBOData
{
	Matrix4x4 model;
	Matrix4x4 view;
	Matrix4x4 projection;
};
void RenderSystemVK::Init()
{
	VulkanInstance::CreateInfo instanceCI;
#ifdef VKB_DEBUG
	instanceCI.EnableValidation = true;
#endif

	m_Instance = VulkanInstance::Create(instanceCI);
	VkPhysicalDevice vkPhysicalDevice = m_Instance->GetVkPhysicalDevices().at(0);

	std::unique_ptr<VulkanPhysicalDevice> physicalDevice = VulkanPhysicalDevice::Create(vkPhysicalDevice, *m_Instance);

	//目前只创建了用于图形显示的队列
	std::vector<VkDeviceQueueCreateInfo> queueInfos(1);
	std::vector<float>                   queuePriorities(1);
	queuePriorities[0] = 1.0f;

	VkDeviceQueueCreateInfo& queueCI = queueInfos[0];
	queueCI.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	queueCI.flags = 0; // reserved for future use
	queueCI.queueFamilyIndex = physicalDevice->FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
	queueCI.queueCount = 1;
	queueCI.pQueuePriorities = queuePriorities.data();

	VkDeviceCreateInfo vkDeviceCreateInfo{};
	vkDeviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	vkDeviceCreateInfo.enabledLayerCount = 0;       // 已废弃
	vkDeviceCreateInfo.ppEnabledLayerNames = nullptr; // 已废弃
	vkDeviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueInfos.size());
	vkDeviceCreateInfo.pQueueCreateInfos = queueInfos.data();

	const auto& vkDeviceFeatures = physicalDevice->GetFeatures();
	VkPhysicalDeviceFeatures vkEnabledFeatures{};
	vkDeviceCreateInfo.pEnabledFeatures = &vkEnabledFeatures;

	std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
	if (physicalDevice->IsExtensionSupported(VK_KHR_MAINTENANCE1_EXTENSION_NAME))
		deviceExtensions.push_back(VK_KHR_MAINTENANCE1_EXTENSION_NAME); // 支持反转viewport

	vkDeviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.empty() ? nullptr : deviceExtensions.data();
	vkDeviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());

	using ExtensionFeatures = VulkanPhysicalDevice::ExtensionFeatures;
	ExtensionFeatures        EnabledExtFeats = {};

	auto vkAllocator = m_Instance->GetVkAllocator();
	m_Device = VulkanDevice::Create(*physicalDevice, vkDeviceCreateInfo, EnabledExtFeats, vkAllocator);

	//glfwInit();

	//glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	//auto window = glfwCreateWindow(1400, 900, "Vulkan", nullptr, nullptr);
	//
	//glfwSetWindowUserPointer(window, this); 
	//
	//HWND hwnd = glfwGetWin32Window(window);
	//glfwSetWindowUserPointer(window, this);
	NativeWindow win;
	m_SwapChain = std::make_shared<VulkanSwapChain>(m_Instance->GetSharedPtr(), m_Device->GetSharedPtr(), win, 1400, 900);


	CreateDepthStencil();
	CreateRenderPass();
	CreateFrameBuffer();
	CreateSemaphores();
	CreateFences();
	CreateCommandBuffers();
	CreateMeshBuffers();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSetLayout();
	CreateDescriptorSet();
	CreatePipelines();
	SetupCommandBuffers();
}

void Spectre::RenderSystemVK::Loop()
{
	while (true) {
		glfwPollEvents();
		Draw();
	}

	//vkDeviceWaitIdle(device);
}

void Spectre::RenderSystemVK::CreateDepthStencil()
{
	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();
	VkDevice device = m_Device->GetVkDevice();

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageCreateInfo.extent = { fwidth,fheight, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.flags = 0;
	vkCreateImage(device, &imageCreateInfo, nullptr, &m_DepthStencilImage);

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.image = m_DepthStencilImage;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;

	VkMemoryRequirements memRequire;
	vkGetImageMemoryRequirements(device, imageViewCreateInfo.image, &memRequire);
	uint32_t memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memRequire.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkMemoryAllocateInfo memAllocateInfo = {};
	memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocateInfo.allocationSize = memRequire.size;
	memAllocateInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_DepthStencilMemory);
	vkBindImageMemory(device, m_DepthStencilImage, m_DepthStencilMemory, 0);

	vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_DepthStencilView);
}

void Spectre::RenderSystemVK::CreateRenderPass()
{
	std::vector<VkAttachmentDescription> attachments(2);
	// color attachment
	attachments[0].format = m_SwapChain->GetSwapChainFormat();
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	// depth stencil attachment
	attachments[1].format = VK_FORMAT_D24_UNORM_S8_UINT;
	attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference colorReference = { };
	colorReference.attachment = 0;
	colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthReference = { };
	depthReference.attachment = 1;
	depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpassDescription = { };
	subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpassDescription.colorAttachmentCount = 1;
	subpassDescription.pColorAttachments = &colorReference;
	subpassDescription.pDepthStencilAttachment = &depthReference;
	subpassDescription.pResolveAttachments = nullptr;
	subpassDescription.inputAttachmentCount = 0;
	subpassDescription.pInputAttachments = nullptr;
	subpassDescription.preserveAttachmentCount = 0;
	subpassDescription.pPreserveAttachments = nullptr;

	std::vector<VkSubpassDependency> dependencies(2);
	dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[0].dstSubpass = 0;
	dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	dependencies[1].srcSubpass = 0;
	dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	dependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpassDescription;
	renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
	renderPassInfo.pDependencies = dependencies.data();
	vkCreateRenderPass(m_Device->GetVkDevice(), &renderPassInfo, nullptr, &m_RenderPass);
}

void Spectre::RenderSystemVK::CreateFrameBuffer()
{
	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();
	VkDevice device = m_Device->GetVkDevice();

	VkImageView attachments[2];
	attachments[1] = m_DepthStencilView;

	VkFramebufferCreateInfo frameBufferCreateInfo = {};
	frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferCreateInfo.renderPass = m_RenderPass;
	frameBufferCreateInfo.attachmentCount = 2;
	frameBufferCreateInfo.pAttachments = attachments;
	frameBufferCreateInfo.width = fwidth;
	frameBufferCreateInfo.height = fheight;
	frameBufferCreateInfo.layers = 1;

	const std::vector<VkImageView>& backbufferViews = m_SwapChain->GetImageViews();

	m_FrameBuffers.resize(backbufferViews.size());
	for (uint32_t i = 0; i < m_FrameBuffers.size(); ++i)
	{
		attachments[0] = backbufferViews[i];
		vkCreateFramebuffer(device, &frameBufferCreateInfo, nullptr, &m_FrameBuffers[i]);
	}
}

void Spectre::RenderSystemVK::CreateSemaphores()
{
	VkDevice device = m_Device->GetVkDevice();
	VkSemaphoreCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore(device, &createInfo, nullptr, &m_RenderComplete);
}

void Spectre::RenderSystemVK::CreateFences()
{
	VkDevice device = m_Device->GetVkDevice();
	uint32_t frameCount = m_SwapChain->GetImageCount();

	VkFenceCreateInfo fenceCreateInfo{};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	m_Fences.resize(frameCount);
	for (uint32_t i = 0; i < m_Fences.size(); ++i)
	{
		vkCreateFence(device, &fenceCreateInfo, nullptr, &m_Fences[i]);
	}
}

void Spectre::RenderSystemVK::CreateCommandBuffers()
{
	VkDevice device = m_Device->GetVkDevice();
	
	VkCommandPoolCreateInfo cmdPoolInfo{};
	cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolInfo.queueFamilyIndex = m_Device->GetPhysicalDevice().FindQueueFamily(VK_QUEUE_GRAPHICS_BIT);
	cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &m_CommandPool);

	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = 1;
	cmdBufferInfo.commandPool = m_CommandPool;

	m_CommandBuffers.resize(m_SwapChain->GetImageCount());
	for (uint32_t i = 0; i < m_CommandBuffers.size(); ++i)
	{
		vkAllocateCommandBuffers(device, &cmdBufferInfo, &(m_CommandBuffers[i]));
	}
}

void Spectre::RenderSystemVK::CreateMeshBuffers()
{
	VkDevice device = m_Device->GetVkDevice();
	VkQueue queue = m_Device->GetGraphicQueue();

	// 顶点数据
	std::vector<Vertex> vertices = {
		{
			{  1.0f,  1.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }
		},
		{
			{ -1.0f,  1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }
		},
		{
			{  0.0f, -1.0f, 0.0f }, { 0.0f, 0.0f, 1.0f }
		}
	};

	// 索引数据
	std::vector<uint16_t> indices = { 0, 1, 2 };
	m_IndicesCount = (uint32_t)indices.size();

	// 顶点数据以及索引数据在整个生命周期中几乎不会发生改变，因此最佳的方式是将这些数据存储到GPU的内存中。
	// 存储到GPU内存也能加快GPU的访问。为了存储到GPU内存中，需要如下几个步骤。
	// 1、在主机端(Host)创建一个Buffer
	// 2、将数据拷贝至该Buffer
	// 3、在GPU端(Local Device)创建一个Buffer
	// 4、通过Transfer簇将数据从主机端拷贝至GPU端
	// 5、删除主基端(Host)的Buffer
	// 6、使用GPU端(Local Device)的Buffer进行渲染
	GPUBuffer tempVertexBuffer;
	GPUBuffer  tempIndexBuffer;

	void* dataPtr = nullptr;
	VkMemoryRequirements memReqInfo;
	VkMemoryAllocateInfo memAllocInfo{};
	memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

	// vertex buffer
	VkBufferCreateInfo vertexBufferInfo{};
	vertexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	vertexBufferInfo.size = vertices.size() * sizeof(Vertex);
	vertexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	vkCreateBuffer(device, &vertexBufferInfo, nullptr, &tempVertexBuffer.buffer);

	vkGetBufferMemoryRequirements(device, tempVertexBuffer.buffer, &memReqInfo);
	uint32_t memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &tempVertexBuffer.memory);
	vkBindBufferMemory(device, tempVertexBuffer.buffer, tempVertexBuffer.memory, 0);

	vkMapMemory(device, tempVertexBuffer.memory, 0, memAllocInfo.allocationSize, 0, &dataPtr);
	std::memcpy(dataPtr, vertices.data(), vertexBufferInfo.size);
	vkUnmapMemory(device, tempVertexBuffer.memory);

	// local device vertex buffer
	vertexBufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	vkCreateBuffer(device, &vertexBufferInfo, nullptr, &m_VertexBuffer.buffer);

	vkGetBufferMemoryRequirements(device, m_VertexBuffer.buffer, &memReqInfo);
	memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_VertexBuffer.memory);
	vkBindBufferMemory(device, m_VertexBuffer.buffer, m_VertexBuffer.memory, 0);

	// index buffer
	VkBufferCreateInfo indexBufferInfo{};
	indexBufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	indexBufferInfo.size = m_IndicesCount * sizeof(uint16_t);
	indexBufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	vkCreateBuffer(device, &indexBufferInfo, nullptr, &tempIndexBuffer.buffer);

	vkGetBufferMemoryRequirements(device, tempIndexBuffer.buffer, &memReqInfo);
	memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &tempIndexBuffer.memory);
	vkBindBufferMemory(device, tempIndexBuffer.buffer, tempIndexBuffer.memory, 0);

	vkMapMemory(device, tempIndexBuffer.memory, 0, memAllocInfo.allocationSize, 0, &dataPtr);
	std::memcpy(dataPtr, indices.data(), indexBufferInfo.size);
	vkUnmapMemory(device, tempIndexBuffer.memory);

	indexBufferInfo.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
	vkCreateBuffer(device, &indexBufferInfo, nullptr, &m_IndicesBuffer.buffer);

	vkGetBufferMemoryRequirements(device, m_IndicesBuffer.buffer, &memReqInfo);
	memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);	
	memAllocInfo.allocationSize = memReqInfo.size;
	memAllocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocInfo, nullptr, &m_IndicesBuffer.memory);
	vkBindBufferMemory(device, m_IndicesBuffer.buffer, m_IndicesBuffer.memory, 0);

	VkCommandBuffer xferCmdBuffer;
	// gfx queue自带transfer功能，为了优化需要使用专有的xfer queue。这里为了简单，先将就用。
	VkCommandBufferAllocateInfo xferCmdBufferInfo{};
	xferCmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	xferCmdBufferInfo.commandPool = m_CommandPool;
	xferCmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	xferCmdBufferInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(device, &xferCmdBufferInfo, &xferCmdBuffer);

	// 开始录制命令
	VkCommandBufferBeginInfo cmdBufferBeginInfo{};
	cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	vkBeginCommandBuffer(xferCmdBuffer, &cmdBufferBeginInfo);

	VkBufferCopy copyRegion = {};
	copyRegion.size = vertices.size() * sizeof(Vertex);
	vkCmdCopyBuffer(xferCmdBuffer, tempVertexBuffer.buffer, m_VertexBuffer.buffer, 1, &copyRegion);

	copyRegion.size = indices.size() * sizeof(uint16_t);
	vkCmdCopyBuffer(xferCmdBuffer, tempIndexBuffer.buffer, m_IndicesBuffer.buffer, 1, &copyRegion);

	// 结束录制
	vkEndCommandBuffer(xferCmdBuffer);

	// 提交命令，并且等待命令执行完毕。
	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &xferCmdBuffer;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = 0;

	VkFence fence = VK_NULL_HANDLE;
	vkCreateFence(device, &fenceInfo, nullptr, &fence);
	vkQueueSubmit(queue, 1, &submitInfo, fence);
	vkWaitForFences(device, 1, &fence, VK_TRUE, MAX_int64);

	vkDestroyFence(device, fence, nullptr);
	vkFreeCommandBuffers(device, m_CommandPool, 1, &xferCmdBuffer);

	vkDestroyBuffer(device, tempVertexBuffer.buffer, nullptr);
	vkFreeMemory(device, tempVertexBuffer.memory, nullptr);
	vkDestroyBuffer(device, tempIndexBuffer.buffer, nullptr);
	vkFreeMemory(device, tempIndexBuffer.memory, nullptr);
}


void Spectre::RenderSystemVK::CreateUniformBuffers()
{
	VkDevice device = m_Device->GetVkDevice();

	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeof(UBOData);
	bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	vkCreateBuffer(device, &bufferInfo, nullptr, &m_MVPBuffer.buffer);

	VkMemoryRequirements memReqInfo;
	vkGetBufferMemoryRequirements(device, m_MVPBuffer.buffer, &memReqInfo);
	uint32_t memoryTypeIndex = m_Device->GetPhysicalDevice().GetMemoryTypeIndex(memReqInfo.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memReqInfo.size;
	allocInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &allocInfo, nullptr, &m_MVPBuffer.memory);
	vkBindBufferMemory(device, m_MVPBuffer.buffer, m_MVPBuffer.memory, 0);

	//m_MVPDescriptor.buffer = m_MVPBuffer.buffer;
	//m_MVPDescriptor.offset = 0;
	//m_MVPDescriptor.range = sizeof(UBOData);

	//Matrix_Identity(m_MVPData.model);
	//Matrix_MoveTo(m_MVPData.model, InitVector(0, 0, 1, 0));

	//Matrix_Identity(m_MVPData.view);
	//m_MVPData.view.r[3].m128_f32[2] = 2.5f;
	///*Matrix_MoveTo(m_MVPData.view, InitVector(0, 0, -2.5f, 0));
	//Matrix_Inverse(m_MVPData.view);*/

	//m_MVPData.projection = Matrix_Perspective(DegreesToRadians(75.0f), 1400.f / 900.f, 0.01f, 3000.0f);

	m_MVPDescriptor.buffer = m_MVPBuffer.buffer;
	m_MVPDescriptor.offset = 0;
	m_MVPDescriptor.range = sizeof(UBOData);

	m_MVPData.model.SetIdentity();
	m_MVPData.model.SetOrigin(0, 0, 0);

	m_MVPData.view.SetIdentity();
	m_MVPData.view.m[3][2] = 2.5f; 

	m_MVPData.projection.SetIdentity();
	m_MVPData.projection.Perspective(DegreesToRadians(75.0f), 1400, 900, 0.01f, 3000.0f);
}

void Spectre::RenderSystemVK::CreateDescriptorPool()
{
	VkDevice device = m_Device->GetVkDevice();

	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = 1;

	VkDescriptorPoolCreateInfo descriptorPoolInfo{};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.poolSizeCount = 1;
	descriptorPoolInfo.pPoolSizes = &poolSize;
	descriptorPoolInfo.maxSets = 1;
	vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &m_DescriptorPool);
}

void Spectre::RenderSystemVK::CreateDescriptorSetLayout()
{
	VkDevice device = m_Device->GetVkDevice();

	VkDescriptorSetLayoutBinding layoutBinding;
	layoutBinding.binding = 0;
	layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	layoutBinding.descriptorCount = 1;
	layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	layoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo descSetLayoutInfo{};
	descSetLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descSetLayoutInfo.bindingCount = 1;
	descSetLayoutInfo.pBindings = &layoutBinding;
	vkCreateDescriptorSetLayout(device, &descSetLayoutInfo, nullptr, &m_DescriptorSetLayout);

	VkPipelineLayoutCreateInfo pipeLayoutInfo{};
	pipeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipeLayoutInfo.setLayoutCount = 1;
	pipeLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
	vkCreatePipelineLayout(device, &pipeLayoutInfo, nullptr, &m_PipelineLayout);
}

void Spectre::RenderSystemVK::CreateDescriptorSet()
{
	VkDevice device = m_Device->GetVkDevice();

	VkDescriptorSetAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_DescriptorSetLayout;
	vkAllocateDescriptorSets(device, &allocInfo, &m_DescriptorSet);

	VkWriteDescriptorSet writeDescriptorSet{};
	writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	writeDescriptorSet.dstSet = m_DescriptorSet;
	writeDescriptorSet.descriptorCount = 1;
	writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	writeDescriptorSet.pBufferInfo = &m_MVPDescriptor;
	writeDescriptorSet.dstBinding = 0;
	vkUpdateDescriptorSets(device, 1, &writeDescriptorSet, 0, nullptr);
}

void Spectre::RenderSystemVK::CreatePipelines()
{
	VkDevice device = m_Device->GetVkDevice();

	VkPipelineCacheCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(device, &createInfo, nullptr, &m_PipelineCache);

	VkPipelineInputAssemblyStateCreateInfo inputAssemblyState{};
	inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	VkPipelineRasterizationStateCreateInfo rasterizationState{};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizationState.cullMode = VK_CULL_MODE_NONE;
	rasterizationState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizationState.depthClampEnable = VK_FALSE;
	rasterizationState.rasterizerDiscardEnable = VK_FALSE;
	rasterizationState.depthBiasEnable = VK_FALSE;
	rasterizationState.lineWidth = 1.0f;

	VkPipelineColorBlendAttachmentState blendAttachmentState[1] = {};
	blendAttachmentState[0].colorWriteMask = (
		VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT |
		VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT
		);
	blendAttachmentState[0].blendEnable = VK_FALSE;

	VkPipelineColorBlendStateCreateInfo colorBlendState{};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.attachmentCount = 1;
	colorBlendState.pAttachments = blendAttachmentState;

	VkPipelineViewportStateCreateInfo viewportState{};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.scissorCount = 1;

	std::vector<VkDynamicState> dynamicStateEnables;
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
	dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
	VkPipelineDynamicStateCreateInfo dynamicState{};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = (uint32_t)dynamicStateEnables.size();
	dynamicState.pDynamicStates = dynamicStateEnables.data();

	VkPipelineDepthStencilStateCreateInfo depthStencilState{};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilState.depthTestEnable = VK_TRUE;
	depthStencilState.depthWriteEnable = VK_TRUE;
	depthStencilState.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilState.depthBoundsTestEnable = VK_FALSE;
	depthStencilState.back.failOp = VK_STENCIL_OP_KEEP;
	depthStencilState.back.passOp = VK_STENCIL_OP_KEEP;
	depthStencilState.back.compareOp = VK_COMPARE_OP_ALWAYS;
	depthStencilState.stencilTestEnable = VK_FALSE;
	depthStencilState.front = depthStencilState.back;

	VkPipelineMultisampleStateCreateInfo multisampleState{};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //todo
	multisampleState.pSampleMask = nullptr;

	// (triangle.vert):
	// layout (location = 0) in vec3 inPos;
	// layout (location = 1) in vec3 inColor;
	// Attribute location 0: Position
	// Attribute location 1: Color
	// vertex input bindding
	VkVertexInputBindingDescription vertexInputBinding = {};
	vertexInputBinding.binding = 0; // Vertex Buffer 0
	vertexInputBinding.stride = sizeof(Vertex); // Position + Color
	vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	std::vector<VkVertexInputAttributeDescription> vertexInputAttributs(2);
	// position
	vertexInputAttributs[0].binding = 0;
	vertexInputAttributs[0].location = 0; // triangle.vert : layout (location = 0)
	vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributs[0].offset = 0;
	// color
	vertexInputAttributs[1].binding = 0;
	vertexInputAttributs[1].location = 1; // triangle.vert : layout (location = 1)
	vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexInputAttributs[1].offset = 12;

	VkPipelineVertexInputStateCreateInfo vertexInputState{};
	vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputState.vertexBindingDescriptionCount = 1;
	vertexInputState.pVertexBindingDescriptions = &vertexInputBinding;
	vertexInputState.vertexAttributeDescriptionCount = 2;
	vertexInputState.pVertexAttributeDescriptions = vertexInputAttributs.data();

	std::vector<VkPipelineShaderStageCreateInfo> shaderStages(2);
	shaderStages[0] = {};
	shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	shaderStages[0].module = LoadSPIPVShader("../../../../../../Resources/Shaders/triangle.vert.spv");
	shaderStages[0].pName = "main";

	shaderStages[1] = {};
	shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	shaderStages[1].module = LoadSPIPVShader("../../../../../../Resources/Shaders/triangle.frag.spv");
	shaderStages[1].pName = "main";

	VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineCreateInfo.layout = m_PipelineLayout;
	pipelineCreateInfo.renderPass = m_RenderPass;
	pipelineCreateInfo.stageCount = (uint32_t)shaderStages.size();
	pipelineCreateInfo.pStages = shaderStages.data();
	pipelineCreateInfo.pVertexInputState = &vertexInputState;
	pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
	pipelineCreateInfo.pRasterizationState = &rasterizationState;
	pipelineCreateInfo.pColorBlendState = &colorBlendState;
	pipelineCreateInfo.pMultisampleState = &multisampleState;
	pipelineCreateInfo.pViewportState = &viewportState;
	pipelineCreateInfo.pDepthStencilState = &depthStencilState;
	pipelineCreateInfo.pDynamicState = &dynamicState;
	vkCreateGraphicsPipelines(device, m_PipelineCache, 1, &pipelineCreateInfo, nullptr, &m_Pipeline);

	vkDestroyShaderModule(device, shaderStages[0].module, nullptr);
	vkDestroyShaderModule(device, shaderStages[1].module, nullptr);
}

void Spectre::RenderSystemVK::SetupCommandBuffers()
{
	VkCommandBufferBeginInfo cmdBeginInfo{};
	cmdBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

	VkClearValue clearValues[2];
	clearValues[0].color = {
		{0.2f, 0.2f, 0.2f, 1.0f}
	};
	clearValues[1].depthStencil = { 1.0f, 0 };

	uint32_t fwidth = m_SwapChain->GetWidth();
	uint32_t fheight = m_SwapChain->GetHeight();

	VkRenderPassBeginInfo renderPassBeginInfo{};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = m_RenderPass;
	renderPassBeginInfo.clearValueCount = 2;
	renderPassBeginInfo.pClearValues = clearValues;
	renderPassBeginInfo.renderArea.offset.x = 0;
	renderPassBeginInfo.renderArea.offset.y = 0;
	renderPassBeginInfo.renderArea.extent.width = fwidth;
	renderPassBeginInfo.renderArea.extent.height = fheight;

	for (uint32_t i = 0; i < m_CommandBuffers.size(); ++i)
	{
		renderPassBeginInfo.framebuffer = m_FrameBuffers[i];

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

		vkBeginCommandBuffer(m_CommandBuffers[i], &cmdBeginInfo);

		vkCmdBeginRenderPass(m_CommandBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
		vkCmdSetViewport(m_CommandBuffers[i], 0, 1, &viewport);
		vkCmdSetScissor(m_CommandBuffers[i], 0, 1, &scissor);
		vkCmdBindDescriptorSets(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 0, nullptr);
		vkCmdBindPipeline(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline);
		vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, &m_VertexBuffer.buffer, offsets);
		vkCmdBindIndexBuffer(m_CommandBuffers[i], m_IndicesBuffer.buffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(m_CommandBuffers[i], m_IndicesCount, 1, 0, 0, 0);
		vkCmdEndRenderPass(m_CommandBuffers[i]);

		vkEndCommandBuffer(m_CommandBuffers[i]);
	}
}
#include <fstream>
static std::vector<char> readFile(const std::string& filename)
{
	std::ifstream file(filename, std::ios::ate | std::ios::binary);

	if (!file.is_open())
	{
		throw std::runtime_error("failed to open file!");
	}
	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}
VkShaderModule Spectre::RenderSystemVK::LoadSPIPVShader(const std::string& filepath)
{
	VkDevice device = m_Device->GetVkDevice();

	std::vector<char> shaderCode = readFile(filepath);

	VkShaderModuleCreateInfo moduleCreateInfo{};
	moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	moduleCreateInfo.codeSize = shaderCode.size();
	moduleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	VkShaderModule shaderModule;
	vkCreateShaderModule(device, &moduleCreateInfo, nullptr, &shaderModule);

	return shaderModule;
}

void Spectre::RenderSystemVK::Draw()
{
	UpdateUniformBuffers();

	VkDevice device = m_Device->GetVkDevice();
	VkQueue queue = m_Device->GetGraphicQueue();
	int32_t backBufferIndex = m_SwapChain->AcquireImageIndex(&m_PresentComplete);

	VkPipelineStageFlags waitStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &waitStageMask;
	submitInfo.pWaitSemaphores = &m_PresentComplete;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &m_RenderComplete;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pCommandBuffers = &(m_CommandBuffers[backBufferIndex]);
	submitInfo.commandBufferCount = 1;

	// 提交绘制命令
	vkResetFences(device, 1, &(m_Fences[backBufferIndex]));
	vkQueueSubmit(queue, 1, &submitInfo, m_Fences[backBufferIndex]);
	vkWaitForFences(device, 1, &(m_Fences[backBufferIndex]), true, 200 * 1000 * 1000);

	// present
	m_SwapChain->Present(
		m_Device->GetGraphicQueue(),
		&m_RenderComplete
	);
}

void Spectre::RenderSystemVK::UpdateUniformBuffers()
{
	VkDevice device = m_Device->GetVkDevice();
	//m_MVPData.model.AppendRotation(90.0f * delta, Vector3::UpVector);
	uint8_t* pData = nullptr;
	int a =  sizeof(UBOData);
	vkMapMemory(device, m_MVPBuffer.memory, 0, sizeof(UBOData), 0, (void**)&pData);
	std::memcpy(pData, &m_MVPData, sizeof(UBOData));
	vkUnmapMemory(device, m_MVPBuffer.memory);
}
