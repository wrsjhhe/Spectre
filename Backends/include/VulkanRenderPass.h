#pragma once
#include "VulkanEngine.h"
BEGIN_NAMESPACE_SPECTRE

class VulkanRenderPass;
typedef std::shared_ptr<VulkanRenderPass> VulkanRenderPassPtr;
class VulkanRenderPass
{
public:
	static std::shared_ptr<VulkanRenderPass> CreateCommonRenderPass(VkFormat colorAttachmentFormat);

public:
	~VulkanRenderPass();
	VkRenderPass GetVkRenderPass()const { return m_VkRenderPass; }

	void Destroy();
private:
	VulkanRenderPass();

	void CreateRenderPass(const std::vector<VkAttachmentDescription>& attachments, const VkSubpassDescription& subpassDescription,
		const std::vector<VkSubpassDependency>& dependencies);
private:
	VkRenderPass								 m_VkRenderPass;
};

END_NAMESPACE_SPECTRE
