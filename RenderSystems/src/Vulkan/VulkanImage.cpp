#include "VulkanCommon.h"
#include "VulkanDevice.h"
#include "VulkanImage.h"

USING_NAMESPACE(Spectre)




std::shared_ptr<VulkanImage> VulkanImage::CreateDepthStencil(const VulkanDevice& vulkanDevice,uint32_t width,uint32_t height)
{
	auto* pImage = new VulkanImage(vulkanDevice);
	
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageCreateInfo.extent = { width,height, 1 };
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
	imageCreateInfo.flags = 0;
	pImage->CreateImage(imageCreateInfo);

	VkImageViewCreateInfo imageViewCreateInfo = {};
	imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	imageViewCreateInfo.format = VK_FORMAT_D24_UNORM_S8_UINT;
	imageViewCreateInfo.flags = 0;
	imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
	imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
	imageViewCreateInfo.subresourceRange.levelCount = 1;
	imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
	imageViewCreateInfo.subresourceRange.layerCount = 1;
	pImage->CreateImageView(imageViewCreateInfo);

	return std::shared_ptr<VulkanImage>(pImage);
}


void VulkanImage::Destroy()
{
	VkDevice device = m_Device.GetVkDevice();
	vkFreeMemory(device, m_ImageMemory, nullptr);
	m_ImageMemory = VK_NULL_HANDLE;

	vkDestroyImageView(device, m_VkImageView, nullptr);
	m_VkImageView = VK_NULL_HANDLE;

	vkDestroyImage(device, m_VkImage, nullptr);
	m_VkImage = VK_NULL_HANDLE;
}

VulkanImage::VulkanImage(const VulkanDevice& vulkanDevice):
	m_Device(vulkanDevice)
{

}

void VulkanImage::CreateImage(const VkImageCreateInfo& imageCI)
{
	vkCreateImage(m_Device.GetVkDevice(), &imageCI, nullptr, &m_VkImage);
}

void VulkanImage::CreateImageView(VkImageViewCreateInfo& imageViewCI)
{
	VkDevice device = m_Device.GetVkDevice();
	imageViewCI.image = m_VkImage;
	VkMemoryRequirements memRequire;
	vkGetImageMemoryRequirements(device, imageViewCI.image, &memRequire);
	uint32_t memoryTypeIndex = m_Device.GetPhysicalDevice().GetMemoryTypeIndex(memRequire.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	VkMemoryAllocateInfo memAllocateInfo = {};
	memAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memAllocateInfo.allocationSize = memRequire.size;
	memAllocateInfo.memoryTypeIndex = memoryTypeIndex;
	vkAllocateMemory(device, &memAllocateInfo, nullptr, &m_ImageMemory);
	vkBindImageMemory(device, m_VkImage, m_ImageMemory, 0);

	vkCreateImageView(device, &imageViewCI, nullptr, &m_VkImageView);

}

