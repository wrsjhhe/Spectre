#pragma once

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

class VulkanImages
{
public:
	static std::shared_ptr<VulkanImages> CreateSwapChainImage(const VulkanSwapChain& swapChain);

	static std::shared_ptr<VulkanImages> CreateDepthStencilImage(uint32_t width, uint32_t height);
public:
	~VulkanImages();

	const std::vector<VkImage>& GetVkImages() const { return m_VkImages; }

	const std::vector <VkImageView>& GetVkImageViews() const { return m_VkImageViews; }

	void Destroy();
private:
	VulkanImages();

	void CreateImages();

	void CreateImageViews();
private:
	std::vector<VkImage>						 m_VkImages;
	std::vector <VkImageView>					 m_VkImageViews;
	std::vector <VkDeviceMemory>                 m_VkDeviceMemory;
	VulkanImageInfo								 m_ImageInfo = {};
};

END_NAMESPACE_SPECTRE