#include "VulkanCommon.h"
#include "VulkanEngine.h"
#include "VulkanSwapchain.h"
#include "VulkanImage.h"

USING_NAMESPACE(Spectre)

std::shared_ptr<VulkanImage> VulkanImage::Create(uint32_t width, uint32_t height, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage)
{
	auto* pImage = new VulkanImage();
	pImage->CreateImage(width, height, format, tiling, usage);
	return std::shared_ptr<VulkanImage>(pImage);
}

void VulkanImage::CreateImage(uint32_t width, uint32_t height, VkFormat format,
	VkImageTiling tiling, VkImageUsageFlags usage)
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkDevice device = pEngine->GetVkDevice();

	//create image
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = format;
	imageCreateInfo.extent = { width,height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = tiling;
	imageCreateInfo.usage = usage;
	imageCreateInfo.flags = 0;

	vkCreateImage(device, &imageCreateInfo, nullptr, &m_VkImage);

	VkMemoryRequirements memRequire;
	vkGetImageMemoryRequirements(device, m_VkImage, &memRequire);
	uint32_t memoryTypeIndex = pEngine->GetMemoryTypeIndex(memRequire.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkMemoryAllocateInfo memAllocateInfo = {};
	memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocateInfo.allocationSize = memRequire.size;
	memAllocateInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_VkDeviceMemory);
	vkBindImageMemory(device, m_VkImage, m_VkDeviceMemory, 0);
}

//void VulkanImage::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
//{
//	auto* pEngine = VulkanEngine::GetInstance();
//	VkDevice device = pEngine->GetVkDevice();
//
//	VkImageViewCreateInfo imageViewCreateInfo = {};
//	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
//	imageViewCreateInfo.format = format;
//	imageViewCreateInfo.flags = 0;
//	imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
//	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
//	imageViewCreateInfo.subresourceRange.levelCount = 1;
//	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
//	imageViewCreateInfo.subresourceRange.layerCount = 1;
//
//	vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_VkImageView);
//}

//
//std::shared_ptr<VulkanImage> VulkanImage::CreateSwapChainImage(const VulkanSwapChain& swapChain)
//{
//	auto* pEngine = VulkanEngine::GetInstance();
//	uint32_t imageCount = swapChain.GetImageCount();
//	auto* pImage = new VulkanImage();
//
//	pImage->m_ImageInfo.Count = imageCount;
//	pImage->m_ImageInfo.ImageAutoDestory = true;
//	pImage->m_ImageInfo.Width = swapChain.GetWidth();
//	pImage->m_ImageInfo.Height = swapChain.GetHeight();
//	pImage->m_ImageInfo.Format = g_SurfaceFormat.format;
//	pImage->m_ImageInfo.AspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
//
//	pImage->m_VkImages.resize(imageCount);
//	vkGetSwapchainImagesKHR(pEngine->GetVkDevice(), swapChain.GetVkSwapChain(), &imageCount, pImage->m_VkImages.data());
//
//	pImage->CreateImageViews();
//
//	return std::shared_ptr<VulkanImage>(pImage);
//}
//
//std::shared_ptr<VulkanImage> VulkanImage::CreateDepthStencilImage(uint32_t width, uint32_t height)
//{
//	auto* pImage = new VulkanImage();
//
//	pImage->m_ImageInfo.Count = 1;
//	pImage->m_ImageInfo.Width = width;
//	pImage->m_ImageInfo.Height = height;
//	pImage->m_ImageInfo.Format = VK_FORMAT_D24_UNORM_S8_UINT;
//	pImage->m_ImageInfo.Usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
//	pImage->m_ImageInfo.AspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
//
//	pImage->CreateImages();
//
//	pImage->CreateImageViews();
//
//	return std::shared_ptr<VulkanImage>(pImage);
//}

VulkanImage::~VulkanImage()
{
	Destroy();
}
void VulkanImage::Destroy()
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkDevice device = pEngine->GetVkDevice();
	vkFreeMemory(device, m_VkDeviceMemory, nullptr);
	m_VkDeviceMemory = VK_NULL_HANDLE;

	vkDestroyImage(device, m_VkImage, nullptr);
	m_VkImage = VK_NULL_HANDLE;

	/*auto* pEngine = VulkanEngine::GetInstance();
	VkDevice device = pEngine->GetVkDevice();

	for (uint32_t i = 0; i < m_ImageInfo.Count; ++i)
	{
		if (m_VkDeviceMemory.size() > i)
		{
			vkFreeMemory(device, m_VkDeviceMemory[i], nullptr);
			m_VkDeviceMemory[i] = VK_NULL_HANDLE;
		}
			
		vkDestroyImageView(device, m_VkImageViews[i], nullptr);
		m_VkImageViews[i] = VK_NULL_HANDLE;

		if (!m_ImageInfo.ImageAutoDestory)
			vkDestroyImage(device, m_VkImages[i], nullptr);
		m_VkImages[i] = VK_NULL_HANDLE;
	}
	m_VkImageViews.clear();
	m_VkImages.clear();
	m_ImageInfo.Count = 0;*/
}

//
//void VulkanImage::CreateImages()
//{
//	auto* pEngine = VulkanEngine::GetInstance();
//	VkDevice device = pEngine->GetVkDevice();
//
//	VkImageCreateInfo imageCreateInfo = {};
//	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
//	imageCreateInfo.imageType = m_ImageInfo.ImageType;
//	imageCreateInfo.format = m_ImageInfo.Format;
//	imageCreateInfo.extent = { m_ImageInfo.Width,m_ImageInfo.Height, 1 };
//	imageCreateInfo.mipLevels = m_ImageInfo.MipLevels;
//	imageCreateInfo.arrayLayers = 1;
//	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
//	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
//	imageCreateInfo.usage = m_ImageInfo.Usage;
//	imageCreateInfo.flags = 0;
//
//	m_VkImages.resize(m_ImageInfo.Count);
//	m_VkDeviceMemory.resize(m_ImageInfo.Count);
//	for (uint32_t i = 0; i < m_ImageInfo.Count; ++i)
//	{
//		vkCreateImage(device, &imageCreateInfo, nullptr, &m_VkImages[i]);
//
//		VkMemoryRequirements memRequire;
//		vkGetImageMemoryRequirements(device, m_VkImages[i], &memRequire);
//		uint32_t memoryTypeIndex = pEngine->GetMemoryTypeIndex(memRequire.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
//
//		VkMemoryAllocateInfo memAllocateInfo = {};
//		memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
//		memAllocateInfo.allocationSize = memRequire.size;
//		memAllocateInfo.memoryTypeIndex = memoryTypeIndex;
//		vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_VkDeviceMemory[i]);
//		vkBindImageMemory(device, m_VkImages[i], m_VkDeviceMemory[i], 0);
//	}
//
//}
//
//void VulkanImage::CreateImageViews()
//{
//	auto* pEngine = VulkanEngine::GetInstance();
//	VkDevice device = pEngine->GetVkDevice();
//
//	VkImageViewCreateInfo imageViewCreateInfo = {};
//	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
//	imageViewCreateInfo.viewType = m_ImageInfo.ViewType;
//	imageViewCreateInfo.format = m_ImageInfo.Format;
//	imageViewCreateInfo.flags = 0;
//	imageViewCreateInfo.subresourceRange.aspectMask = m_ImageInfo.AspectMask;
//	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
//	imageViewCreateInfo.subresourceRange.levelCount = 1;
//	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
//	imageViewCreateInfo.subresourceRange.layerCount = 1;
//
//	m_VkImageViews.resize(m_ImageInfo.Count);
//	for (uint32_t i = 0; i < m_ImageInfo.Count; ++i)
//	{
//		imageViewCreateInfo.image = m_VkImages[i];
//		vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_VkImageViews[i]);
//	}
//}

std::shared_ptr<VulkanImageView> VulkanImageView::Create(VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
	VulkanImageView* pImageView = new VulkanImageView();
	pImageView->CreateImageView(image,format, aspectMask);

	return std::shared_ptr<VulkanImageView>(pImageView);
}

VulkanImageView::~VulkanImageView()
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkDevice device = pEngine->GetVkDevice();

	vkDestroyImageView(device, m_VkImageView, nullptr);
	m_VkImageView = VK_NULL_HANDLE;
}

void VulkanImageView::CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask)
{
	auto* pEngine = VulkanEngine::GetInstance();
	VkDevice device = pEngine->GetVkDevice();

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = format;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.subresourceRange.aspectMask = aspectMask;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	imageViewCreateInfo.image = image;

	vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_VkImageView);
}
