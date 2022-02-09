#pragma once

#include <vector>
BEGIN_NAMESPACE_SPECTRE

class VulkanRenderPass
{
public:
	static std::shared_ptr<VulkanRenderPass> CreateCommonRenderPass(const VulkanDevice& vulkanDevice, VkFormat colorAttachmentFormat);

public:
	VkRenderPass GetRenderPass()const { return m_VkRenderPass; }

	void Destroy();
private:
	VulkanRenderPass(const VulkanDevice& vulkanDevice);

	void CreateRenderPass(const std::vector<VkAttachmentDescription>& attachments, const VkSubpassDescription& subpassDescription,
		const std::vector<VkSubpassDependency>& dependencies);
private:
	const VulkanDevice&							 m_Device;
	VkRenderPass								 m_VkRenderPass;
};

END_NAMESPACE_SPECTRE
