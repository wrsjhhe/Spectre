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
}

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
