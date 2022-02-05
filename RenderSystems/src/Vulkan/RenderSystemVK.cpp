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
	queueCI.queueFamilyIndex = physicalDevice->FindQueueFamily(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);
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

	glfwInit();

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

	auto window = glfwCreateWindow(800, 600, "Vulkan", nullptr, nullptr);
	
	glfwSetWindowUserPointer(window, this); VK_PIPELINE_BIND_POINT_GRAPHICS;
	
	HWND hwnd = glfwGetWin32Window(window);

	NativeWindow win(hwnd);
	m_SwapChain = std::make_shared<VulkanSwapChain>(m_Instance->GetSharedPtr(), m_Device->GetSharedPtr(), win, 800, 600);


	CreateDepthStencil();
	CreateRenderPass();
	CreateFrameBuffer();
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
