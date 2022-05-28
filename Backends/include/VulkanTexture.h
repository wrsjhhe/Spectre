#pragma once
#include "VulkanCommon.h"
#include "VulkanImage.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanTexture;
typedef std::shared_ptr<VulkanTexture> VulkanTexturePtr;
class VulkanTexture
{
public:
	static VulkanTexturePtr CreaetInvalid();
	static VulkanTexturePtr Create2D(unsigned char* rgbData, int width, int height);

	~VulkanTexture();
	VulkanImageViewPtr GetImageView() const { return m_ImageView; }
	VkSampler GetVkSampler() const { return m_Sampler; }

private:
	void Create(unsigned char* rgbData, int width, int height);
private:
	void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void CreateTextureSampler();
private:
	VulkanImagePtr					m_Image;
	VulkanImageViewPtr				m_ImageView;
	VkSampler						m_Sampler;
};

END_NAMESPACE_SPECTRE