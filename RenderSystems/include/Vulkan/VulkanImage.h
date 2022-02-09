#pragma once

BEGIN_NAMESPACE_SPECTRE

struct VulkanImageInfo
{
	VkImageType			ImageType = VK_IMAGE_TYPE_2D;
	VkFormat			Format = VK_FORMAT_D24_UNORM_S8_UINT;
	uint32_t			Width = 0;
	uint32_t			Height = 0;
	uint32_t			MipLevels = 1;
	VkImageUsageFlags	Usage = VK_IMAGE_USAGE_FLAG_BITS_MAX_ENUM;
};

class VulkanImage
{
public:

	static std::shared_ptr<VulkanImage> CreateDepthStencil(const VulkanDevice& vulkanDevice,
		 uint32_t width, uint32_t height);
public:
	VkImage GetVkImage() const { return m_VkImage; }

	VkImageView GetVkImageView() const { return m_VkImageView; }

	inline void GetVkMemoryRequirements(VkMemoryRequirements& memRequire);

	void Destroy();
private:
	VulkanImage(const VulkanDevice& vulkanDevice);

	void CreateImage(const VkImageCreateInfo& imageCI);

	void CreateImageView(VkImageViewCreateInfo& imageViewCI);
private:
	const VulkanDevice&							 m_Device;
	VkImage										 m_VkImage = VK_NULL_HANDLE;
	VkImageView									 m_VkImageView = VK_NULL_HANDLE;
	VkDeviceMemory								 m_ImageMemory = VK_NULL_HANDLE;
	VulkanImageInfo								 m_ImageInfo;
};

END_NAMESPACE_SPECTRE