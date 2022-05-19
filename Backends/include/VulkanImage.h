#pragma once
#include <VulkanCommon.h>

BEGIN_NAMESPACE_SPECTRE

struct VulkanImageInfo
{
	uint32_t            Count = 0;
	bool                ImageAutoDestory = false;
	VkImageType			ImageType = VK_IMAGE_TYPE_2D;
	VkImageViewType     ViewType = VK_IMAGE_VIEW_TYPE_2D;
	VkFormat			Format = VK_FORMAT_B8G8R8A8_UNORM;
	uint32_t			Width = 0;
	uint32_t			Height = 0;
	uint32_t			MipLevels = 1;
	VkImageUsageFlags	Usage = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
	VkImageAspectFlags  AspectMask = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;
};

class VulkanSwapChain;
class VulkanImage;
typedef std::shared_ptr<VulkanImage> VulkanImagePtr;
class VulkanImage
{
public:
	static std::shared_ptr<VulkanImage> Create(uint32_t width, uint32_t height, VkFormat format,
		VkImageTiling tiling, VkImageUsageFlags usage);

private:
	void CreateImage(uint32_t width, uint32_t height, VkFormat format, 
		VkImageTiling tiling, VkImageUsageFlags usage);
public:
	~VulkanImage();

	const VkImage& GetVkImage() const { return m_VkImage; }

	void Destroy();
private:
	VkImage                         m_VkImage = VK_NULL_HANDLE;
	VkDeviceMemory                  m_VkDeviceMemory = VK_NULL_HANDLE;
};

class VulkanImageView;
typedef std::shared_ptr<VulkanImageView> VulkanImageViewPtr;
class VulkanImageView
{
public:
	static std::shared_ptr<VulkanImageView> Create(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);
	~VulkanImageView();
	const VkImageView& GetVkImageView() const { return m_VkImageView; }
private:

	void CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectMask);

private:
	VkImageView                     m_VkImageView = VK_NULL_HANDLE;
};

END_NAMESPACE_SPECTRE